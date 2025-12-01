# SYSC4001_A3_P2 - Concurrent TA Marking System

A concurrent system simulating multiple Teaching Assistants (TAs) marking student exams, demonstrating the importance of proper synchronization in concurrent programming.

## Overview

This project implements a concurrent system simulating multiple Teaching Assistants (TAs) marking student exams. The system demonstrates the importance of proper synchronization in concurrent programming by providing two implementations:

### Part 2a: Unsynchronized Version
- **File**: `ta_marking_basic.cpp`
- **Purpose**: Demonstrate race conditions
- **Synchronization**: None (intentionally omitted)
- **Expected Behavior**: Data races, inconsistent results
- **Learning Objective**: Understand why synchronization is necessary

### Part 2b: Synchronized Version
- **File**: `ta_marking_semaphores.cpp`
- **Purpose**: Correct concurrent implementation
- **Synchronization**: std::mutex (readers-writers pattern)
- **Expected Behavior**: No race conditions, consistent results
- **Learning Objective**: Proper synchronization techniques

## System Requirements

### Software Requirements

- **Compiler:** g++ 7.0+ with C++17 support
- **Operating System:** Linux or macOS
- **Threading Library:** pthread (usually included)
- **Standard Library:** C++17 STL

### Hardware Requirements

- **CPU:** Any modern processor with multi-core support
- **RAM:** Minimum 512 MB
- **Disk:** 10 MB for source and output

## Dependencies
```bash
# Ubuntu
sudo apt-get update
sudo apt-get install build-essential g++

# macOS (requires Homebrew)
brew install gcc
```

## Project Structure
```
SYSC4001_A3P2/
│
├── ta_marking_basic_<student1>_<student2>.cpp         # Part 2a: No sync
├── ta_marking_semaphores_<student1>_<student2>.cpp    # Part 2b: With sync
│
├── Makefile                                            # Build system
├── build.sh                                            # Build script
│
├── exam_files/                                         # Exam data
│   ├── exam_1.txt
│   ├── exam_2.txt
│   ├── ...
│   └── exam_9999.txt
│
├── rubric.txt                                          # Marking rubric
│
├── reportPartC.pdf                                     # Analysis report
└── README.md                                           # This file
```

## Installation

### Clone the Repository
```bash
git clone 
cd SYSC4001_A3P2
```

### Create Exam Files
```bash
# Option 1: Using provided script
./create_exams.sh

# Option 2: Manual creation
for i in {1..25}; do echo $(printf "%04d" $i) > exam_$i.txt; done
echo "9999" > exam_9999.txt
```

### Create Rubric File
```bash
cat > rubric.txt << EOF
1, A
2, B
3, C
4, D
5, E
EOF
```

## Compilation

### Option 1: Using Makefile
```bash
# Build both versions
make all

# Build individually
make basic       # Part 2a
make semaphores  # Part 2b

# Clean build artifacts
make clean
```

### Option 2: Using build.sh
```bash
chmod +x build.sh
./build.sh
```

### Option 3: Manual Compilation
```bash
# Part 2a: Basic version (no synchronization)
g++ -Wall -Wextra -g -std=c++17 -pthread \
    -o ta_marking_basic ta_marking_basic.cpp

# Part 2b: Synchronized version
g++ -Wall -Wextra -g -std=c++17 -pthread \
    -o ta_marking_semaphores ta_marking_semaphores.cpp
```

**Important:** The `-pthread` flag is required for threading support.

## Usage

### Running Part 2a (Unsynchronized)
```bash
# Run with 2 TAs
./ta_marking_basic 2

# Run with 4 TAs
./ta_marking_basic 4

# Run with default (2 TAs)
./ta_marking_basic
```

### Running Part 2b (Synchronized)
```bash
# Run with 3 TAs
./ta_marking_semaphores 3

# Run with 5 TAs
./ta_marking_semaphores 5

# Run with 10 TAs
./ta_marking_semaphores 10
```

### Command-Line Arguments
```
./ta_marking_<version> [num_tas]

Arguments:
  num_tas  - Number of concurrent TAs (default: 2, minimum: 2)
```

### Expected Output
```
Starting TA marking system with 3 TAs (with synchronization)
TA 1: Started
TA 2: Started
TA 3: Started
TA 1: Reviewing rubric for exam 1
TA 2: Reviewing rubric for exam 1
TA 3: Reviewing rubric for exam 1
TA 1: Reading rubric line 1: 1, A
TA 2: Reading rubric line 1: 1, A
TA 3: Reading rubric line 1: 1, A
TA 1: Needs to correct rubric question 2
TA 1: Writing correction to rubric question 2
TA 1: Corrected rubric for question 2: B -> C
TA 1: Marking question 1 for student 1
TA 2: Marking question 2 for student 1
TA 3: Marking question 3 for student 1
TA 1: Completed marking question 1 for student 1
...
All TAs have finished. Marking complete.
```

## 📌 Notes About Exam Files

### The assignment originally describes multiple exam text files (exam_1.txt, exam_2.txt, …).
###  This C++ implementation simulates exams entirely in memory using:

### exam_data.student_number


### Student numbers increment from 1 → 9999

### No external exam files are required
