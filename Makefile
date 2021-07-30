GXX=g++
GXXFLAGS=-lspot -std=c++17 -w -g
DEPS = Horn.h
OBJ = main.o Horn.o

%.o: %.cpp $(DEPS)
	$(GXX) -c -o $@ $< $(GXXFLAGS)

SSyftHorn: $(OBJ)
	$(GXX) -o $@ $^ $(GXXFLAGS)


.PHONY: clean

clean:
	rm -f *.o *~