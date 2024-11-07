# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic

# Source and executables
SRC = pgm.cpp
EXEC = pgm
COMPARE_EXEC = compare_files

# Files for testing
MY_OUTPUT = employee.tbl

# Default target: Compile both pgm and compare_files
all: build compare_files

# Build the main program (pgm)
build: $(EXEC)

$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRC)
	@echo "Built $(EXEC) executable."

# Compile the compare_files utility
compare_files: compare_files.cpp
	$(CXX) $(CXXFLAGS) -o $(COMPARE_EXEC) compare_files.cpp
	@echo "Built $(COMPARE_EXEC) utility."

# Run the main program with a CSV file input
run: $(EXEC)
	@if [ -z "$(CSV_FILE)" ]; then \
		echo "Usage: make run CSV_FILE=<path_to_csv_file>"; \
	else \
		echo "Running $(EXEC) with CSV file $(CSV_FILE)..."; \
		./$(EXEC) $(CSV_FILE); \
	fi

# Test target: Compare output file to the professor's file
test: compare_files
	@if [ -z "$(FILE)" ]; then \
		echo "Usage: make test FILE=<professor's file>"; \
	else \
		echo "Comparing $(MY_OUTPUT) with $$FILE..."; \
		./$(COMPARE_EXEC) $(MY_OUTPUT) $$FILE; \
	fi

# Clean up build files
clean:
	rm -f $(EXEC) $(COMPARE_EXEC) $(MY_OUTPUT)
	@echo "Cleaned up build files and output."

# Help target to describe each Makefile action
help:
	@echo "Available make commands:"
	@echo "  make all         				- Compile both pgm and compare_files"
	@echo "  make build      				- Build the main program (pgm only)"
	@echo "  make run        				- Run the main program (pgm)"
	@echo "  make test FILE=<professor's file> 	    	- Compare output .tbl file with the professor's .tbl file"
	@echo "  make clean       				- Remove executables and generated .tbl files"
	@echo "  make help        				- Display this help message"

# Phony targets to avoid conflicts with actual files
.PHONY: all build compare_files run test clean help
