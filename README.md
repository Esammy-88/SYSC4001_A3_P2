# SYSC4001_A3_P2
This project implements a concurrent system simulating multiple Teaching Assistants (TAs) marking student exams. The system demonstrates the importance of proper synchronization in concurrent programming by providing two implementations.
🔹 Part 2a — Unsynchronized Version

Multiple TAs run concurrently

Access shared rubric and exam data without synchronization

Race conditions are intentional, demonstrating:

Inconsistent writes

Duplicated marking

Corrupted rubric entries

🔹 Part 2b — Synchronized Version

Uses C++17 <mutex> to simulate semaphore-based coordination.

Implements:

Readers–Writers pattern for rubric access

Per-question mutexes to prevent double marking

Exclusive lock for loading the next exam

This version prevents all race conditions seen in Part 2a.

✨ Common Behavior (Both Programs)

Review a 5-question rubric

Randomly correct one rubric entry

Mark exams one question at a time

Load the next exam

Stop automatically at student 9999

📁 File Structure
.
├── ta_marking_basic.cpp          # Part 2a (no synchronization)
├── ta_marking_semaphores.cpp     # Part 2b (with synchronization)
├── Makefile                      # Build automation
├── build.sh                      # Optional build script
├── rubric.txt                    # Auto-generated rubric file
└── README.md

🛠️ How to Compile
Option 1 — Using the Makefile

Build everything:

make


Clean all binaries:

make clean

Option 2 — Using build.sh

Make the script executable and run it:

chmod +x build.sh
./build.sh


This script:

Creates a rubric.txt file

Compiles both Part 2a and Part 2b programs

▶️ How to Run
Part 2a — Unsynchronized Version

Run with 2 TAs:

./ta_marking_basic 2


Example with more TAs:

./ta_marking_basic 4

Part 2b — Synchronized Version

Run with 3 TAs:

./ta_marking_semaphores 3


Example with more TAs:

./ta_marking_semaphores 6

🧪 Test Cases

Try different numbers of TAs:

./ta_marking_basic 2
./ta_marking_basic 5
./ta_marking_semaphores 3
./ta_marking_semaphores 8

Expected Results
Part 2a (Unsynchronized)

Corrupted rubric entries

Two or more TAs marking the same question

Multiple TAs loading the next exam

Interleaved and inconsistent console output

Part 2b (Synchronized)

Clean, predictable behavior

Correct rubric updates

No duplicated marking

Exactly one TA loads the next exam

All TAs stop at exam 9999

📌 Notes About Exam Files

The assignment originally describes multiple exam text files (exam_1.txt, exam_2.txt, …).
This C++ implementation simulates exams entirely in memory using:

exam_data.student_number


Student numbers increment from 1 → 9999

No external exam files are required
