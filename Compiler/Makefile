CC = g++

SUBDIRS := $(shell ls -F | grep "\/")
DIRS := ./ $(SUBDIRS)
SOURCE_FILES := $(foreach d, $(DIRS), $(wildcard $(d)*.cpp))
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCE_FILES))

all: crayon

crayon: $(OBJECTS)
	$(CC) -std=c++11 -Werror -Wall -pedantic -o crayon $(OBJECTS)

clean:
	rm crayon *.o util/*.o tokens/*.o parser/*.o resolver/*.o
