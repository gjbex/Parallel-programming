CXX = g++
CXXFLAGS = -std=c++17 -O2 -g -Wall -Wextra -Wpedantic
LDLIBS = -lm

.PRECIOUS: %.o

all: julia_abs.exe julia.exe julia_tbb.exe

%_tbb.exe: %_tbb.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -ltbb $(LDLIBS)

%.exe: %.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(wildcard *.o) $(wildcard *.exe)
	$(RM) core $(wildcard core.*)
