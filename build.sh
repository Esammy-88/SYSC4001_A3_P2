#!/bin/bash

echo "Building TA Marking System..."

# Create rubric file
cat > rubric.txt << EOF
1, A
2, B
3, C
4, D
5, E
EOF
echo "✓ rubric.txt created"

# Compile Part 2a
g++ -Wall -std=c++17 -pthread -o ta_marking_basic ta_marking_basic.cpp
echo "✓ ta_marking_basic compiled"

# Compile Part 2b
g++ -Wall -std=c++17 -pthread -o ta_marking_semaphores ta_marking_semaphores.cpp
echo "✓ ta_marking_semaphores compiled"

echo ""
echo "Build complete!"
echo "Usage:"
echo "  ./ta_marking_basic 2"
echo "  ./ta_marking_semaphores 3"
