CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: GenAlloc CheckAlloc

GenAlloc: main.o Parser.o Allocator.o Score.o
	$(CXX) $(CXXFLAGS) -o GenAlloc main.o Parser.o Allocator.o Score.o

CheckAlloc: CheckAlloc.o Parser.o
	$(CXX) $(CXXFLAGS) -o CheckAlloc CheckAlloc.o Parser.o

main.o: main.cpp Parser.h Allocator.h Score.h Staff.h Project.h Student.h
	$(CXX) $(CXXFLAGS) -c main.cpp

CheckAlloc.o: CheckAlloc.cpp Parser.h Staff.h Project.h Student.h
	$(CXX) $(CXXFLAGS) -c CheckAlloc.cpp

Parser.o: Parser.cpp Parser.h Staff.h Project.h Student.h
	$(CXX) $(CXXFLAGS) -c Parser.cpp

Allocator.o: Allocator.cpp Allocator.h Staff.h Project.h Student.h
	$(CXX) $(CXXFLAGS) -c Allocator.cpp

Score.o: Score.cpp Score.h Staff.h Project.h Student.h
	$(CXX) $(CXXFLAGS) -c Score.cpp

clean:
	rm -f *.o GenAlloc CheckAlloc
