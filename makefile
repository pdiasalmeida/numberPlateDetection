CC=g++
CFLAGS=-c -g -O0 -Wall
LDFLAGS=-lopencv_core -lopencv_highgui -lopencv_imgproc
SOURCES=Plate.cpp ShapeBasedDetector.cpp EdgeBasedDetector.cpp lp_detector.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lp_detector

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)