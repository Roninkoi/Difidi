CXX = g++
CXXFLAGS = -std=c++11 -llapack -lgsl -lgslcblas -lm

SOURCES = $(wildcard src/*.cpp)
HEADERS = $(wildcard src/*.h)
OBJECTS = $(notdir $(SOURCES:.cpp=.o))

release: CXXFLAGS += -O3
release: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o difidi

debug: CXXFLAGS += -ggdb
debug: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o difidi_debug

$(OBJECTS): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -c

all:
	make release
	make debug

clean:
	rm -rf *.o difidi difidi_debug

.PHONY: clean all
.DEFAULT: release

