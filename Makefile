CXX := g++
CXXFLAGS := -Iinclude -Wall -Wextra -std=c++17 -g
SRC_DIR := src
INCLUDE_DIR := include
OBJ_DIR := obj
TEST_SCRIPT := scripts/run_differential.sh

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TARGET := trabalho
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	bash $(TEST_SCRIPT)

.PHONY: all clean run test
