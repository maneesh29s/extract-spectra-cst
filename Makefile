CXXFLAGS=-std=c++14 -Iinclude/

OPTIMISATION=-O3
# OPTIMISATION=-fsanitize=address -g

LIB=-ljsoncpp
CXX=g++

main: src/main

test: test/test_spectralExtractor

all: main test

# src
src/%: src/%.cc dir
	$(CXX) $(CXXFLAGS) $(OPTIMISATION) $(LIB) -o build/$@ $< $(OPT)

# test
test/%: test/%.cc dir
	$(CXX) $(CXXFLAGS) $(OPTIMISATION) $(LIB) -o build/$@ $< $(OPT)


dir:
	mkdir -p build/test build/src out/

.PHONY: dir

clean:
	rm -rf build/*