
CXX = g++
CXXFLAGS = -lncurses -Wall -Wextra -O2
DEBUG_FLAGS = -g

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

OUT = $(BUILD_DIR)/tetris

SRCS = main.cpp Util.cpp Tetris.cpp Tetromino.cpp Playfield.cpp
OBJS = $(SRCS:.cpp=.o)
BUILD_SRCS = $(addprefix $(BUILD_DIR)/, $(OBJS))

# -----------------------

always: build

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: build

prep:
	@mkdir -p $(BUILD_DIR)

build: prep $(OUT)

$(OUT): $(BUILD_SRCS)
	$(CXX) $^ $(CXXFLAGS) -o $(OUT)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -I$(INCLUDE_DIR) -c $(CXXFLAGS) -o $@ $<

run: build
	@./$(OUT)

format:
	astyle --style=attach --indent=spaces=4 $(SRC_DIR)/*.cpp $(INCLUDE_DIR)/*.hpp

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Clean!"

.PHONY: build run
