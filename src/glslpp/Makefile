all: glslpp

clean:
	rm -f parserpp.cpp parserpp.hpp tokenspp.cpp glslpp

parserpp.cpp: parserpp.y
	bison -t -d -o $@ $^

parserpp.hpp: parserpp.cpp

tokenspp.cpp: tokenspp.l parserpp.hpp
	flex -d -o $@ $^

glslpp: parserpp.cpp main.cpp tokenspp.cpp
	g++ -o $@ $^

.PHONY: all clean
