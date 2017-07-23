SOURCES = main.cpp Horn.cpp
OBJS = main.o Horn.o

GXX=g++
GXXFLAGS= -std=c++11 -w -g


SSyftHorn: $(SOURCES)
	$(GXX) $(SOURCES) $(GXXFLAGS) 
	$(GXX) -o SSyftHorn $(SOURCES) $(GXXFLAGS) 




.PHONY: SSyftHorn
clean:
	rm  SSyftHorn
