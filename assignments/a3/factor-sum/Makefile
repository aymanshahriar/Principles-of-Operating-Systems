SOURCES = main.cpp sumFactors.cpp
CPPC = g++
CPPFLAGS = -c -Wall -O2
LDLIBS = -pthread -lm
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = sumFactors

all: $(TARGET)

sumFactors.o: sumFactors.h
main.o: sumFactors.h
%.o : %.c
$(OBJECTS): Makefile 

.cpp.o:
	$(CPPC) $(CPPFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CPPC) -o $@ $(OBJECTS) $(LDLIBS)

.PHONY: clean
clean:
	rm -f *~ *.o $(TARGET) 

