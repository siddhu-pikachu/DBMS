#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <array>
using namespace std;

class EmployeeRecord {
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

        EmployeeRecord(int id, const string& ssn, const string& firstName,
                   char middleInitial, const string& lastName, const string& birthDate,
                   const string& address, char sex, int salary, short deptNumber)
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

        array<char, 112> serialize() const{

            array<char, 112> buffer = {0};
            int offset = 0;

            memcpy(&buffer[offset], &rowId, 4);
            offset+=4;

            memcpy(&buffer[offset], ssn.c_str(), 9);
            offset+=9;

            memcpy(&buffer[offset], firstName.c_str(), 20);
            offset += 20;

            memcpy(&buffer[offset], &middleInitial, 1);
            offset += 1;

            memcpy(&buffer[offset], lastName.c_str(), 20);
            offset += 20;

            memcpy(&buffer[offset], birthDate.c_str(), 10);
            offset += 10;

            memcpy(&buffer[offset], address.c_str(), 40);
            offset += 40;

            memcpy(&buffer[offset], &sex, 1);
            offset += 1;

            memcpy(&buffer[offset], &salary, 4);
            offset += 4;

            memcpy(&buffer[offset], &departmentNumber, 2);
            offset += 2;

            memcpy(&buffer[offset], &deletionMarker, 1);
            offset += 1;

            return buffer;
        }
};

class Page {
public:
    static const int PAGE_SIZE = 4096;    // Page size in bytes
    int pageNumber;
    std::vector<EmployeeRecord> records;

    Page(int num) : pageNumber(num) {}

    // Add a record to the page (simple check to avoid overflow)
    bool addRecord(const EmployeeRecord& record) {
        if (records.size() * 112 >= PAGE_SIZE - 2) {  // -2 for header space
            return false;
        }
        records.push_back(record);
        return true;
    }

    // Serialize the page
    std::vector<char> serialize() const {
        std::vector<char> buffer(PAGE_SIZE, 0);
        int offset = 2;  // Start after header (2 bytes for record count)

        // Write the number of records in this page (2 bytes)
        short recordCount = records.size();
        std::memcpy(&buffer[0], &recordCount, sizeof(recordCount));

        // Write each record to the page
        for (const auto& record : records) {
            auto recordBytes = record.serialize();
            if (offset + recordBytes.size() <= PAGE_SIZE) {
                std::copy(recordBytes.begin(), recordBytes.end(), buffer.begin() + offset);
                offset += recordBytes.size();
            }
        }

        return buffer;
    }
};

class Page {

    private:

        const int pageNumber;
        fstream& fileStream;
        int freeSpaceOffset = PAGE_SIZE;

    public:

        static const int PAGE_SIZE = 4096;
        static const int RECORD_SIZE = 112;
        static const int HEADER_SIZE = 2;

        Page(int num, std::fstream& stream)
            : pageNumber(num), fileStream(stream), freeSpaceOffset(PAGE_SIZE) {}

        // Initialize the page with a fixed structure
        void initialize() {
            fileStream.seekp(pageNumber * PAGE_SIZE, std::ios::beg);
            short recordCount = 0;
            fileStream.write(reinterpret_cast<char*>(&recordCount), sizeof(recordCount));
            freeSpaceOffset = PAGE_SIZE; // Start from the highest available space
        }
        
};
int main (){
    cout<<"hi";
}