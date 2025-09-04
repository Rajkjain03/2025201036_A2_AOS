# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -g -Wall
LDFLAGS =

# Directories
EXEC_DIR = executableFiles
HEADER_DIR = headerFiles
BUILD_DIR = objFiles

all:
	@make -q $(TARGET) && echo "Target '$(TARGET)' is already up to date." || make --no-print-directory $(TARGET)


# Executable name
TARGET = myshell

# Find all .cpp source files in the source directory
SRCS = $(wildcard $(EXEC_DIR)/*.cpp)
# Generate object file names from source file names
OBJS = $(patsubst $(EXEC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# The default target: build the executable
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile source files into object files
# This creates the build directory if it doesn't exist
$(BUILD_DIR)/%.o: $(EXEC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "Please Wait. Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(HEADER_DIR) -c $< -o $@

# Rule to clean up build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Cleanup complete."
# Phony targets are not files
.PHONY: all clean