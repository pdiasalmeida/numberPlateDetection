CC=g++
CFLAGS=-c -g -O0 -Wall `pkg-config --cflags opencv`
LDFLAGS=`pkg-config --libs opencv` -ltesseract
SOURCES=detection/ShapeBasedDetector.cpp detection/EdgeBasedDetector.cpp recognition/OCRClassifier.cpp lp_detector.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lp_detector

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) ./results/*
