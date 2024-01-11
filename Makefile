CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -g -O0
OPENCV_LIB = -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc
DIR_OPENCV = -I/usr/local/include/opencv4

all: tp1.o

tp1.o: obj/tp1.o obj/Region.o obj/Seed.o obj/ComputeRegions.o obj/ComputeSeed.o 
	$(CXX) $(CXXFLAGS) obj/tp1.o obj/Region.o obj/Seed.o obj/ComputeRegions.o obj/ComputeSeed.o -o tp1.o $(OPENCV_LIB)

obj/tp1.o: src/main.cpp src/fonctions.hpp src/ComputeRegions.hpp src/Region.hpp src/Seed.hpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o obj/tp1.o  $(DIR_OPENCV)

obj/Region.o: src/Region.cpp src/Region.hpp
	$(CXX) $(CXXFLAGS) -c src/Region.cpp -o obj/Region.o $(DIR_OPENCV)

obj/Seed.o: src/Seed.cpp src/Seed.hpp
	$(CXX) $(CXXFLAGS) -c src/Seed.cpp -o obj/Seed.o $(DIR_OPENCV)

obj/ComputeRegions.o: src/ComputeRegions.cpp src/ComputeRegions.hpp src/ComputeSeed.hpp src/Region.hpp
	$(CXX) $(CXXFLAGS) -c src/ComputeRegions.cpp -o obj/ComputeRegions.o $(DIR_OPENCV)

obj/ComputeSeed.o: src/ComputeSeed.cpp src/ComputeSeed.hpp src/Seed.hpp
	$(CXX) $(CXXFLAGS) -c src/ComputeSeed.cpp -o obj/ComputeSeed.o $(DIR_OPENCV)

clean:
	rm obj/*.o
	rm *.o

.DEFAULT_GOAL := all

.PHONY: all clean
