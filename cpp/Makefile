CC=g++
CFLAGS=-c -Wall -I lib -g -ggdb -std=c++11
LDFLAGS=-I../../includes -I../../includes/nifticlib-2.0.0/include
LIBFLAGS=-L../../includes/nifticlib-2.0.0/lib -lfslio -lnifticdf -lniftiio -lznz -lz
OUTDIR=bin/linux
SOURCES=\
demo/source/DataPointCollection.cpp\
demo/source/RegressionGaussianNd.cpp\
demo/source/Graphics.cpp\
demo/source/CommandLineParser.cpp\
demo/source/dibCodec.cpp\
demo/source/GaussianAggregator.cpp\
demo/source/FeatureResponseFunctions.cpp\
demo/source/PlotCanvas.cpp\
demo/source/Platform.cpp\
demo/source/main.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(OUTDIR)/sw

all: $(SOURCES) $(EXECUTABLE)	
	cp -R demo/data $(OUTDIR)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(OUTDIR)    
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean: 
	rm -f sw $(OBJECTS)
	rm -r -f $(OUTDIR)

