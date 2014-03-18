CC=			g++
CFLAGS=		-c -g -O0 -Wall
LDFLAGS=	
SOURCES=	main.cpp
OBJECTS=	$(SOURCES:.cpp=.o)
EXECUTABLE=	lp_detector_evaluator

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
