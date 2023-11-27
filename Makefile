CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -I/usr/local/include/opencv4 -g -MMD

SRC_DIR = src
OBJ_DIR = obj
EXECUTABLE = tp0

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

$(EXECUTABLE): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc -Wl,--no-as-needed -l:libQt5Core.so.5 -l:libQt5Gui.so.5 -l:libQt5Widgets.so.5 -mno-pconfig

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Include automatically generated dependencies
-include $(OBJ_FILES:.o=.d)

all: $(EXECUTABLE)

clean:
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d $(EXECUTABLE)

.PHONY: all clean
