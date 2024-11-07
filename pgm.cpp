#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <array>
#include <algorithm>
#include <map>
#include <sstream>
#include <bit>
#include <cstdint>
#include <string>
using namespace std;

// Convert a 4-byte integer to big-endian format
uint32_t toBigEndian(uint32_t value) {
    return ((value & 0xFF000000) >> 24) |
           ((value & 0x00FF0000) >> 8)  |
           ((value & 0x0000FF00) << 8)  |
           ((value & 0x000000FF) << 24);
}

// Convert a 2-byte short integer to big-endian format
uint16_t toBigEndian(uint16_t value) {
    return ((value & 0xFF00) >> 8) |
           ((value & 0x00FF) << 8);
}

class EmployeeRecord
{
private:
    int rowId;
    string ssn;
    string firstName;
    char middleInitial;
    string lastName;
    string birthDate;
    string address;
    char sex;
    int salary;
    short departmentNumber;
    char deletionMarker;

public:
    EmployeeRecord(int id, const string &ssn, const string &firstName,
                   char middleInitial, const string &lastName, const string &birthDate,
                   const string &address, char sex, int salary, short deptNumber)
        : rowId(id), ssn(ssn), firstName(firstName), middleInitial(middleInitial), lastName(lastName),
          birthDate(birthDate), address(address), sex(sex), salary(salary), departmentNumber(deptNumber),
          deletionMarker(0) {}

    int getRowId() const { return rowId; }
    string getSsn() const { return ssn; }
    string getFirstName() const { return firstName; }
    char getMiddleInitial() const { return middleInitial; }
    string getLastName() const { return lastName; }
    string getBirthDate() const { return birthDate; }
    string getAddress() const { return address; }
    char getSex() const { return sex; }
    int getSalary() const { return salary; }
    short getDepartmentNumber() const { return departmentNumber; }
    char getDeletionMarker() const { return deletionMarker; }

    array<char, 112> serialize() const
    {

        array<char, 112> buffer = {0};
        int offset = 0;
        
        // converting rowId to big endian format for writing it 
        uint32_t temp_rowId = toBigEndian(static_cast<uint32_t>(rowId));
        memcpy(&buffer[offset], &(temp_rowId), 4);
        offset += 4;

        //write the ssn
        memcpy(&buffer[offset], ssn.c_str(), 9);
        offset += 9;

        // Write First Name (20 bytes, padded if necessary)
        memcpy(&buffer[offset], firstName.c_str(), min(firstName.size(), size_t(20)));
        offset += 20;

        memcpy(&buffer[offset], &middleInitial, 1);
        offset += 1;

        // Write Last Name (20 bytes, padded if necessary)
        memcpy(&buffer[offset], lastName.c_str(), min(lastName.size(), size_t(20)));
        offset += 20;

        // Write Birth Date (10 bytes, padded if necessary)
        memcpy(&buffer[offset], birthDate.c_str(), min(birthDate.size(), size_t(10)));
        offset += 10;

        // Write Address (40 bytes, padded if necessary)
        memcpy(&buffer[offset], address.c_str(), min(address.size(), size_t(40)));
        offset += 40;

        // write the sex
        memcpy(&buffer[offset], &sex, 1);
        offset += 1;

        // converting salary to big endian format for writing it 
        uint32_t temp_salary = toBigEndian(static_cast<uint32_t>(salary));
        memcpy(&buffer[offset], &temp_salary, 4);
        offset += 4;

        // converting departmentNumber to big endian format for writing it 
        uint16_t temp_departmentNumber = toBigEndian(static_cast<uint16_t>(departmentNumber));
        memcpy(&buffer[offset], &temp_departmentNumber, 2);
        offset += 2;

        // writing the deletion marker 
        memcpy(&buffer[offset], &deletionMarker, 1);
        offset += 1;

        return buffer;
    }

    // Setter for rowId
    void setRowId(int id)
    {
        rowId = id;
    }
};

class Page
{
public:
    static const int RECORD_SIZE = 112;     // record size
    static const int PAGE_SIZE = 4096;      // Page size in bytes
    static const int HEADER_SIZE = 2;       // header size

private:
    int pageNumber;
    fstream &fileStream;                    // Reference to the shared file stream
    int freeSpaceOffset;                    // Tracks the highest available offset for the next record
    // vector<EmployeeRecord> records;      // Stores records in memory for this page
    short int recordCount = 0;              // 2 byte integer for record count
    map<string, short int> ssnToOffsetMap;  // Maps `ssn` to record offsets, sorted by `ssn`

public:
    Page(int num, fstream &stream)
        : pageNumber(num), fileStream(stream), freeSpaceOffset(PAGE_SIZE) {}

    // Initialize the page with a fixed structure
    void initialize()
    {
        fileStream.seekp(pageNumber * PAGE_SIZE, ios::beg);
        recordCount = 0;
        //since we are writing 0, no need to conver it to big endian
        fileStream.write(reinterpret_cast<char *>(&recordCount), sizeof(recordCount)); // writing the record count to the file in binary
        freeSpaceOffset = PAGE_SIZE;                                                   // Start from the highest available space
    }

    // Start from the highest available space
    bool addRecord(const EmployeeRecord &record)
    {
        if (freeSpaceOffset - RECORD_SIZE - HEADER_SIZE - (recordCount + 1) * 2 < 0)
        {
            return false; // Not enough space
        }

        freeSpaceOffset -= RECORD_SIZE;
        ssnToOffsetMap[record.getSsn()] = freeSpaceOffset;

        // write the record to the file immediately to its offset
        fileStream.seekp(pageNumber * PAGE_SIZE + freeSpaceOffset, ios::beg);
        auto recordData = record.serialize();
        fileStream.write(recordData.data(), RECORD_SIZE);

        // increment the record count after adding a new record and update the header
        recordCount++;
        updateHeader();

        return true;
    }

private:
    void updateHeader()
    {
        // update the header with the new record count and sorted offsets
        fileStream.seekp(pageNumber * PAGE_SIZE, ios::beg);
        uint16_t temp_recordCount = toBigEndian(static_cast<uint16_t>(recordCount));                    //converting to big endian format before writing it 
        fileStream.write(reinterpret_cast<char *>(&temp_recordCount), sizeof(temp_recordCount));
        // fileStream.write(reinterpret_cast<char *>(&recordCount), sizeof(recordCount));               //commented out: little endian format

        // write the sorted offsets by the key 'ssn' into the header
        int offsetPosition = pageNumber * PAGE_SIZE + HEADER_SIZE;
        for (const auto &[ssn, offset] : ssnToOffsetMap)
        {
            fileStream.seekp(offsetPosition, ios::beg);
            uint16_t temp_offset = toBigEndian(static_cast<uint16_t>(offset));                           //converting to big endian format before writing it 
            fileStream.write(reinterpret_cast<char *>(&temp_offset), sizeof(temp_offset));
            // fileStream.write(reinterpret_cast<const char *>(&offset), sizeof(offset));                //commented out: little endian format
            offsetPosition += 2;
        }
    }
};

class Table
{
private:
    fstream fileStream;             // the shared file stream
    vector<Page> pages;             // vector of page objects
    int nextRowId = 1;              // intializing the row id to 1

public:
    // constructor for table
    Table(const string &filename)
    {
        initialize(filename);

        for (int i = 0; i < 10; ++i)
        {
            pages.emplace_back(i, fileStream);  // directly constructs a new element to the end of the vetor
            pages[i].initialize();              // intializes each of the 10 page objects
        }
    }

    void initialize(const string &filename)
    {
        // open file with trucation to start fresh every time
        fileStream.open(filename, ios::in | ios::out | ios::binary | ios::trunc);
        if (!fileStream.is_open())
        {
            cerr << "Error opening file." << endl;
            return;
        }

        // Write 40960 bytes of 0x00 to clear and set the file size
        vector<char> emptyData(40960, 0x00);                               // Create a vector of 40960 bytes initialized to 0x00
        fileStream.write(emptyData.data(), emptyData.size());
        fileStream.flush();                                                // Instantly writing it to disk

        // Reopen the file with read and write access
        fileStream.close();
        fileStream.open(filename, ios::in | ios::out | ios::binary);
    }

    int hashFunction(const string &ssn) const
    {
        // Check if ssn has at least 4 characters
        if (ssn.size() < 4) {
            cerr << "Invalid SSN format: " << ssn << endl;
            return 0; // Default to page 0 if the SSN is invalid
        }

        // Extract the last 4 characters of ssn
        string lastFour = ssn.substr(ssn.size() - 4);

        // Check if the last four characters are digits
        if (!all_of(lastFour.begin(), lastFour.end(), ::isdigit)) {
            cerr << "Invalid SSN format: " << ssn << endl;
            return 0; // Default to page 0 if the SSN format is invalid
        }

        // Convert last 4 characters to an integer and apply modulo for page number
        int key = stoi(lastFour);
        return key % 10;
    }

    void processCSV(const string &csvFilename)
    {
        ifstream csvFile(csvFilename);
        if (!csvFile.is_open())
        {
            cerr << "Error opening CSV file." << endl;
        }

        string line;

        // 'burn' the header line
        if (getline(csvFile, line)) {
            if (line == "Fname,Minit,Lname,Ssn,Bdate,Address,Sex,Salary,Dno") {
                cout << "Skipping header: " << line << endl; // Optional: print the header for confirmation
            } else {
                cerr << "Unexpected file format: Header does not match expected format." << endl;
                return;  // Optional: return if header does not match the expected format
            }
        }

        while (getline(csvFile, line))
        {
            int rowId = assignRowId();
            
            EmployeeRecord record = parseCSVLine(line, rowId);          // create a new record for the line

            int pageNumber = hashFunction(record.getSsn());             // new record's page number

            if (!pages[pageNumber].addRecord(record))
            {
                cerr << "Page " << pageNumber << " is full. Cannot add record." << endl;
            }
        }
        csvFile.close();
    }

private:

    // Helper function to remove dashes from SSN
    string removeDashes(const string& ssn) {
        string cleanSsn;
        for (char ch : ssn) {
            if (ch != '-') {
                cleanSsn += ch;
            }
        }
        return cleanSsn;
    }
    // Parses a CSV line into an EmployeeRecord object
    EmployeeRecord parseCSVLine(const string &line, int rowId)
    {
        stringstream ss(line);
        string field;

        // Extract and parse each field from the line
        string ssn, firstName, lastName, birthDate, address;
        char middleInitial, sex;
        int salary;
        short departmentNumber;

        // CSV format: Fname,Minit,Lname,Ssn,Bdate,Address,Sex,Salary,Dno
        getline(ss, firstName, ',');
        if (ss.peek() != ',') {            // Check if middle initial is available
            ss >> middleInitial;
        }
        ss.ignore(1); // Ignore comma
        getline(ss, lastName, ',');
        getline(ss, ssn, ',');
        // Remove dashes from Ssn
        ssn = removeDashes(ssn);
        getline(ss, birthDate, ',');
        getline(ss, address, ',');
        ss >> sex;
        ss.ignore(1); // Ignore comma
        ss >> salary;
        ss.ignore(1); // Ignore comma
        ss >> departmentNumber;

        return EmployeeRecord(rowId, ssn, firstName, middleInitial, lastName, birthDate, address, sex, salary, departmentNumber);
    }

    // Assign a unique, incremental row ID
    int assignRowId()
    {
        return nextRowId++;
    }
};

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <csv_file>" << std::endl;
        return 1;
    }

    std::string csvFileName = argv[1];
    
    // Specify the name of the .tbl file and the CSV file
    string tblFilename = "employee.tbl";
    string csvFilename = "employee.csv";

    // Create a Table instance, which will initialize the .tbl file
    Table employeeTable(tblFilename);

    // Process the CSV file and add records to the .tbl file
    employeeTable.processCSV(csvFilename);

    cout << "CSV data has been processed and written to " << tblFilename << "." << endl;

    return 0;
}