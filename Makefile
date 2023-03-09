CXXFLAGS=-std=c++14 -Iinclude/

# OPTIMISATION=-O3
OPTIMISATION=-fsanitize=address -g

# LIB=-ljsoncpp
CXX=g++

# SRC = src/

test: test/test_spectralExtractor

all: $(SRC) $(TEST)

# test
test/%: test/%.cc dir
	$(CXX) $(CXXFLAGS) $(OPTIMISATION) $(LIB) -o build/$@ $<


dir:
	mkdir -p build/test

.PHONY: dir

clean:
	rm -rf build/*