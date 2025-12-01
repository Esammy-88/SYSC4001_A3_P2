CXX = g++
CXXFLAGS = -Wall -std=c++17 -pthread

BINS = ta_marking_basic ta_marking_semaphores

all: $(BINS)

ta_marking_basic: ta_marking_basic.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

ta_marking_semaphores: ta_marking_semaphores.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

run_basic:
	./ta_marking_basic 2

run_sync:
	./ta_marking_semaphores 3

clean:
	rm -f $(BINS) rubric.txt

.PHONY: all clean run_basic run_sync
