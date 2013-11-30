
TOP := ../..
INC_DIRS := . ../common $(TOP)/include

# Note that this is not resolved until used below--you can modify INC_DIRS anytime between here and the 
# rules below. 
INC = $(foreach d,$(INC_DIRS),-I$d)



all: glslpp

clean:
	rm -f parserpp.cpp parserpp.hpp tokenspp.cpp glslpp

parserpp.cpp: parserpp.y
	bison -t -d -o $@ $^

parserpp.hpp: parserpp.cpp

tokenspp.cpp: tokenspp.l parserpp.hpp
	flex -d -o $@ $^

glslpp: parserpp.cpp main.cpp tokenspp.cpp preproc.cpp
	g++ -g -o $@ $(INC) $^

.PHONY: all clean
