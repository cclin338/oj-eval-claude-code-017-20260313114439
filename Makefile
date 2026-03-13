CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

code: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o code

clean:
	rm -f code *.o *.dat *.bin

.PHONY: clean
