CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

GenAlloc: main.o Parser.o Allocator.o Score.o
	$(CXX) $(CXXFLAGS) -o GenAlloc main.o Parser.o Allocator.o Score.o

main.o: main.cpp Parser.h Allocator.h Score.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Parser.o: Parser.cpp Parser.h Staff.h Project.h Student.h
	$(CXX) $(CXXFLAGS) -c Parser.cpp

Allocator.o: Allocator.cpp Allocator.h Staff.h Project.h Student.h
	$(CXX) $(CXXFLAGS) -c Allocator.cpp

Score.o: Score.cpp Score.h Staff.h Project.h Student.h
	$(CXX) $(CXXFLAGS) -c Score.cpp

clean:
	rm -f *.o GenAlloc
	