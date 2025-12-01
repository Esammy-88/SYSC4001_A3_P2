 /**
  * TA Marking System with Semaphores
  * This version uses semaphores and mutexes to synchronize access to shared resources
  * 
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>
#include <algorithm>

#define NUM_QUESTIONS 5

// Shared data structures
struct ExamData {
    int student_number;
    std::vector<int> questions_marked;  // 0 = not marked, 1 = being marked, 2 = marked
    bool current_exam_loaded;
    
    ExamData() : student_number(1), questions_marked(NUM_QUESTIONS, 0), current_exam_loaded(true) {}
};

struct RubricData {
    std::vector<std::string> rubric_lines;
    
    RubricData() {
        rubric_lines = {"1, A", "2, B", "3, C", "4, D", "5, E"};
    }
};

// Synchronization primitives
std::mutex rubric_write_mutex;      // Exclusive write access to rubric
std::mutex rubric_read_mutex;       // Protect reader count
std::mutex exam_load_mutex;         // Loading next exam
std::vector<std::mutex> question_mutexes(NUM_QUESTIONS);  // One per question
std::mutex console_mutex;           // For clean console output

int reader_count = 0;

// Global shared data
ExamData exam_data;
RubricData rubric_data;

// Random delay helper
void random_delay(int min_ms, int max_ms) {
    static thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(min_ms, max_ms);
    int delay = distribution(generator);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

// Save rubric to file
void save_rubric() {
    std::ofstream file("rubric.txt");
    if(file.is_open()) {
        for(const auto &line : rubric_data.rubric_lines) {
            file << line << "\n";
        }
        file.close();
    }
}

// Start reading rubric (reader)
void start_read_rubric() {
    rubric_read_mutex.lock();
    reader_count++;
    if(reader_count == 1) {
        rubric_write_mutex.lock();  // First reader locks writers
    }
    rubric_read_mutex.unlock();
}

// End reading rubric
void end_read_rubric() {
    rubric_read_mutex.lock();
    reader_count--;
    if(reader_count == 0) {
        rubric_write_mutex.unlock();  // Last reader unlocks writers
    }
    rubric_read_mutex.unlock();
}

// Start writing rubric (writer)
void start_write_rubric() {
    rubric_write_mutex.lock();
}

// End writing rubric
void end_write_rubric() {
    rubric_write_mutex.unlock();
}

// Correct a rubric line
void correct_rubric_line(int question_num, int ta_id) {
    std::string &line = rubric_data.rubric_lines[question_num];
    size_t comma_pos = line.find(',');
    
    if(comma_pos != std::string::npos && comma_pos + 2 < line.length()) {
        char current_char = line[comma_pos + 2];
        line[comma_pos + 2] = current_char + 1;
        
        {
            std::lock_guard<std::mutex> lock(console_mutex);
            std::cout << "TA " << ta_id << ": Corrected rubric for question " 
                      << question_num + 1 << ": " << current_char << " -> " 
                      << line[comma_pos + 2] << std::endl;
        }
        
        save_rubric();
    }
}

// Load next exam
void load_next_exam() {
    int next_student = exam_data.student_number + 1;
    if(next_student > 9999) next_student = 9999;
    
    exam_data.student_number = next_student;
    exam_data.current_exam_loaded = true;
    for(int i = 0; i < NUM_QUESTIONS; i++) {
        exam_data.questions_marked[i] = 0;
    }
}

// TA process function
void ta_process(int ta_id, int num_tas) {
    std::mt19937 rng(std::random_device{}() + ta_id);
    std::uniform_int_distribution<int> dist(0, 99);
    
    {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << "TA " << ta_id << ": Started" << std::endl;
    }
    
    while(true) {
        // Check if we're done
        if(exam_data.student_number == 9999) {
            std::lock_guard<std::mutex> lock(console_mutex);
            std::cout << "TA " << ta_id << ": Exam 9999 reached, terminating" << std::endl;
            break;
        }
        
        // Review rubric
        {
            std::lock_guard<std::mutex> lock(console_mutex);
            std::cout << "TA " << ta_id << ": Reviewing rubric for exam " 
                      << exam_data.student_number << std::endl;
        }
        
        for(int q = 0; q < NUM_QUESTIONS; q++) {
            // Read rubric
            start_read_rubric();
            
            {
                std::lock_guard<std::mutex> lock(console_mutex);
                std::cout << "TA " << ta_id << ": Reading rubric line " << q + 1 
                          << ": " << rubric_data.rubric_lines[q] << std::endl;
            }
            
            random_delay(500, 1000);
            
            bool needs_correction = (dist(rng) < 20);  // 20% chance
            end_read_rubric();
            
            if(needs_correction) {
                // Write rubric (exclusive access)
                start_write_rubric();
                
                {
                    std::lock_guard<std::mutex> lock(console_mutex);
                    std::cout << "TA " << ta_id << ": Writing correction to rubric question " 
                              << q + 1 << std::endl;
                }
                
                correct_rubric_line(q, ta_id);
                end_write_rubric();
            }
        }
        
        // Mark questions
        bool marked_any = false;
        for(int q = 0; q < NUM_QUESTIONS; q++) {
            std::lock_guard<std::mutex> lock(question_mutexes[q]);
            
            if(exam_data.questions_marked[q] == 0) {
                exam_data.questions_marked[q] = 1;
                
                {
                    std::lock_guard<std::mutex> console_lock(console_mutex);
                    std::cout << "TA " << ta_id << ": Marking question " << q + 1 
                              << " for student " << exam_data.student_number << std::endl;
                }
                
                // Mark the question (outside the question mutex to allow others to check)
                random_delay(1000, 2000);
                
                // Update status
                {
                    std::lock_guard<std::mutex> lock2(question_mutexes[q]);
                    exam_data.questions_marked[q] = 2;
                }
                
                {
                    std::lock_guard<std::mutex> console_lock(console_mutex);
                    std::cout << "TA " << ta_id << ": Completed marking question " << q + 1 
                              << " for student " << exam_data.student_number << std::endl;
                }
                
                marked_any = true;
                break;
            }
        }
        
        // Check if all questions are marked
        bool all_marked = true;
        for(int q = 0; q < NUM_QUESTIONS; q++) {
            if(exam_data.questions_marked[q] != 2) {
                all_marked = false;
                break;
            }
        }
        
        if(all_marked && !marked_any) {
            std::lock_guard<std::mutex> lock(exam_load_mutex);
            
            // Double-check after acquiring lock
            all_marked = true;
            for(int q = 0; q < NUM_QUESTIONS; q++) {
                if(exam_data.questions_marked[q] != 2) {
                    all_marked = false;
                    break;
                }
            }
            
            if(all_marked) {
                {
                    std::lock_guard<std::mutex> console_lock(console_mutex);
                    std::cout << "TA " << ta_id << ": All questions marked, loading next exam" 
                              << std::endl;
                }
                load_next_exam();
            }
        }
        
        if(!marked_any) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

int main(int argc, char **argv) {
    int num_tas = 2;
    
    if(argc > 1) {
        num_tas = std::stoi(argv[1]);
        if(num_tas < 2) num_tas = 2;
    }
    
    std::cout << "Starting TA marking system with " << num_tas << " TAs (with synchronization)" 
              << std::endl;
    
    // Initialize rubric file
    save_rubric();
    
    // Create TA threads
    std::vector<std::thread> ta_threads;
    for(int i = 0; i < num_tas; i++) {
        ta_threads.emplace_back(ta_process, i + 1, num_tas);
    }
    
    // Wait for all TAs to finish
    for(auto &thread : ta_threads) {
        thread.join();
    }
    
    std::cout << "All TAs have finished. Marking complete." << std::endl;
    
    return 0;
}