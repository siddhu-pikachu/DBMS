# Employee Database Program

This project is a C++ program designed to process a CSV file of employee data and store it in a `.tbl` format file. The project includes a testing utility to compare the generated `.tbl` file with a reference file.

## Project Files

- **pgm.cpp**: Main program that processes the CSV file and outputs the data in a `.tbl` file format.
- **compare_files.cpp**: Utility program to test the output `.tbl` file against a reference `.tbl` file.

## Requirements

- **C++ Version**: C++20
- **Compiler**: g++-14

## Usage

1. **Compiling the Program**  
   Run `make all` to compile both `pgm.cpp` and `compare_files.cpp`:
   ```bash
   make all
   ```
   
2. **Running the Program**
   - Place your CSV file in the project directory.
   - Use the following command to run the program, specifying the CSV file:
     ```bash
     make run CSV_FILE=<csvfilename>
     ```
   - Example:
     ```bash
     make run CSV_FILE=employee.csv
     ```

3. **Testing the Output**
   - Place the professor's reference `.tbl` file in the project directory.
   - Run the following command to compare your generated `.tbl` file with the reference file:
     ```bash
     make test FILE=<reference_filename>
     ```
   - Example:
     ```bash
     make test FILE=test.tbl
     ```

4. **Cleaning Up**
   - To remove the executables and generated `.tbl` files, run:
     ```bash
     make clean
     ```

## Makefile Commands

- `make all` - Compiles `pgm` and `compare_files`.
- `make build` - Compiles only `pgm`.
- `make run CSV_FILE=<csvfilename>` - Runs `pgm` with the specified CSV file.
- `make test FILE=<reference_filename>` - Compares the generated `.tbl` file with the reference file.
- `make clean` - Removes the compiled executables and output files.
- `make help` - Displays available commands.

---

This `README.md` provides all the essential instructions for building, running, and testing your program. Let me know if you’d like any additional details!