CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -I/usr/local/include/opencv4 -g -O0

SRC_DIR = src
OBJ_DIR = obj
EXECUTABLE = tp0

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

$(EXECUTABLE): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -g -o $@ $^ -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXECUTABLE)

clean:
	rm -rf $(OBJ_DIR)/*.o $(EXECUTABLE)
	rm -rf *.o

.DEFAULT_GOAL := all

.PHONY: all clean
