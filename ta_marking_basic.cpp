
// ==================== Part 2a: Basic Version (Without Synchronization) ====================
/**
 * ta_marking_basic.cpp
 * Part 2a - Without proper synchronization (race conditions expected)
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <mutex>

#define NUM_QUESTIONS 5

struct ExamData {
    int student_number;
    std::vector<int> questions_marked;
    bool current_exam_loaded;
    
    ExamData() : student_number(1), questions_marked(NUM_QUESTIONS, 0), current_exam_loaded(true) {}
};

struct RubricData {
    std::vector<std::string> rubric_lines;
    
    RubricData() {
        rubric_lines = {"1, A", "2, B", "3, C", "4, D", "5, E"};
    }
};

ExamData exam_data;
RubricData rubric_data;
std::mutex console_mutex;  // Only for console output, not for data protection

void random_delay(int min_ms, int max_ms) {
    static thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(min_ms, max_ms);
    int delay = distribution(generator);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void save_rubric() {
    std::ofstream file("rubric.txt");
    if(file.is_open()) {
        for(const auto &line : rubric_data.rubric_lines) {
            file << line << "\n";
        }
        file.close();
    }
}

void correct_rubric_line(int question_num, int ta_id) {
    std::string &line = rubric_data.rubric_lines[question_num];
    size_t comma_pos = line.find(',');
    
    if(comma_pos != std::string::npos && comma_pos + 2 < line.length()) {
        char current_char = line[comma_pos + 2];
        line[comma_pos + 2] = current_char + 1;
        
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << "TA " << ta_id << ": Corrected rubric for question " 
                  << question_num + 1 << ": " << current_char << " -> " 
                  << line[comma_pos + 2] << std::endl;
        
        save_rubric();
    }
}

void load_next_exam() {
    int next_student = exam_data.student_number + 1;
    if(next_student > 9999) next_student = 9999;
    
    exam_data.student_number = next_student;
    exam_data.current_exam_loaded = true;
    for(int i = 0; i < NUM_QUESTIONS; i++) {
        exam_data.questions_marked[i] = 0;
    }
}

void ta_process_basic(int ta_id, int num_tas) {
    std::mt19937 rng(std::random_device{}() + ta_id);
    std::uniform_int_distribution<int> dist(0, 99);
    
    {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << "TA " << ta_id << ": Started" << std::endl;
    }
    
    while(true) {
        if(exam_data.student_number == 9999) {
            std::lock_guard<std::mutex> lock(console_mutex);
            std::cout << "TA " << ta_id << ": Exam 9999 reached, terminating" << std::endl;
            break;
        }
        
        {
            std::lock_guard<std::mutex> lock(console_mutex);
            std::cout << "TA " << ta_id << ": Reviewing rubric for exam " 
                      << exam_data.student_number << std::endl;
        }
        
        for(int q = 0; q < NUM_QUESTIONS; q++) {
            // NO SYNCHRONIZATION - Reading rubric
            random_delay(500, 1000);
            
            if(dist(rng) < 20) {
                // NO SYNCHRONIZATION - Writing rubric (RACE CONDITION!)
                std::lock_guard<std::mutex> lock(console_mutex);
                std::cout << "TA " << ta_id << ": Needs to correct rubric question " 
                          << q + 1 << std::endl;
                correct_rubric_line(q, ta_id);
            }
        }
        
        bool marked_any = false;
        for(int q = 0; q < NUM_QUESTIONS; q++) {
            // NO SYNCHRONIZATION - Checking and marking (RACE CONDITION!)
            if(exam_data.questions_marked[q] == 0) {
                exam_data.questions_marked[q] = 1;
                
                {
                    std::lock_guard<std::mutex> lock(console_mutex);
                    std::cout << "TA " << ta_id << ": Marking question " << q + 1 
                              << " for student " << exam_data.student_number << std::endl;
                }
                
                random_delay(1000, 2000);
                
                exam_data.questions_marked[q] = 2;
                
                {
                    std::lock_guard<std::mutex> lock(console_mutex);
                    std::cout << "TA " << ta_id << ": Completed marking question " << q + 1 
                              << " for student " << exam_data.student_number << std::endl;
                }
                
                marked_any = true;
                break;
            }
        }
        
        bool all_marked = true;
        for(int q = 0; q < NUM_QUESTIONS; q++) {
            if(exam_data.questions_marked[q] != 2) {
                all_marked = false;
                break;
            }
        }
        
        if(all_marked && !marked_any) {
            // NO SYNCHRONIZATION - Loading next exam (RACE CONDITION!)
            {
                std::lock_guard<std::mutex> lock(console_mutex);
                std::cout << "TA " << ta_id << ": All questions marked, loading next exam" 
                          << std::endl;
            }
            load_next_exam();
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
    
    std::cout << "Starting TA marking system with " << num_tas 
              << " TAs (WITHOUT synchronization - expect race conditions)" << std::endl;
    
    save_rubric();
    
    std::vector<std::thread> ta_threads;
    for(int i = 0; i < num_tas; i++) {
        ta_threads.emplace_back(ta_process_basic, i + 1, num_tas);
    }
    
    for(auto &thread : ta_threads) {
        thread.join();
    }
    
    std::cout << "All TAs have finished. Marking complete." << std::endl;
    
    return 0;
}