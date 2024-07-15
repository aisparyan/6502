# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I/usr/local/include -I./src

# Target executable
TARGET = 6502-emulator
TEST_TARGET = test_emulator

# Source files (include src/main.cpp here if used)
SRCS = src/cpu.cpp src/memory.cpp
TEST_SRCS = tests/cpu_test.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

# Path to Google Test libraries
GTEST_LIB = /usr/local/lib

# Build target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Link object files to create test executable
$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(OBJS) $(TEST_OBJS) -L$(GTEST_LIB) -lgtest -lgtest_main -pthread

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(TARGET) $(TEST_TARGET) $(OBJS) $(TEST_OBJS)

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: all clean