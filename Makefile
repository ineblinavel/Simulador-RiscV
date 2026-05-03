CXX := g++
CXXFLAGS := -Iinclude -Wall -Wextra -std=c++17 -g
SRC_DIR := src
INCLUDE_DIR := include
OBJ_DIR := obj
TEST_SCRIPT := scripts/run_differential.sh
SAN_FLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer

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

cppcheck:
	cppcheck -I$(INCLUDE_DIR) --enable=all --inconclusive --std=c++17 --suppress=missingIncludeSystem --suppress=normalCheckLevelMaxBranches --error-exitcode=1 $(SRC_DIR)


sanitize: CXXFLAGS += $(SAN_FLAGS)
sanitize: clean all

.PHONY: all clean run test cppcheck sanitize
