CC=g++
CFLAGS=-c -g -O0 -Wall
CFLAGS=-c -g -O0 -Wall `pkg-config --cflags opencv`
LDFLAGS=`pkg-config --cflags --libs opencv`
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lp_detector

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)