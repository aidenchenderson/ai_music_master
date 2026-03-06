# -------------------- #
# build specifications #
# -------------------- #

CC = gcc
CXX = g++

# search paths
BACKEND_DIR = backend
FRONTEND_DIR = frontend
CORE_DIR = core
EXTERNAL_LIB_DIR = $(BACKEND_DIR)/lib
INCLUDE_PATHS = \
	-I. \
	-I$(BACKEND_DIR)/include \
	-I$(FRONTEND_DIR)/include \
	-I$(EXTERNAL_LIB_DIR) \
	-I$(BACKEND_DIR)/kissfft \
	-I$(CORE_DIR)

# output directories
BUILD_DIR = build
BIN_DIR = bin

# compiler flags
CXXFLAGS = -std=c++23 -Wall $(INCLUDE_PATHS) $(shell pkg-config --cflags libgpiod)
CFLAGS = -Wall $(INCLUDE_PATHS)
# linker flags
LDFLAGS = -pthread -ldl -lm -lncurses -lcurl $(shell pkg-config --libs libgpiod)

# source files
CPP_SRCS = \
	src/main.cpp \
	$(wildcard $(BACKEND_DIR)/src/*.cpp) \
	$(wildcard $(FRONTEND_DIR)/src/*.cpp) \
	$(wildcard $(CORE_DIR)/*.cpp)

C_SRCS = $(wildcard $(BACKEND_DIR)/kissfft/*.c)

SRCS = $(CPP_SRCS) $(C_SRCS)

# object files
OBJS = $(CPP_SRCS:%.cpp=$(BUILD_DIR)/%.o)
OBJS += $(C_SRCS:%.c=$(BUILD_DIR)/%.o)

TARGET = $(BIN_DIR)/guitar_genre_guru

# ----------- #
# build rules #
# ----------- #

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# compile c++ sources
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# compile c sources
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# --------- #
# utilities #
# --------- #

.PHONY: clean rebuild run
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

rebuild: clean all

run: $(TARGET)
	./$(TARGET)
