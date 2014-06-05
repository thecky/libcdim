CC=g++
LIBSOURCE=libcdim.cpp
LIB=libcdim.so

all: $(LIBSOURCE)
	$(CC) -fPIC -shared $(LIBSOURCE) -o $(LIB)

clean:
	rm -rf *.so
