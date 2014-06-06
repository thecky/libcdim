CC=g++
LIBSOURCE=libcdim.cpp
LIBOBJ=libcdim.o
LIB=libcdim.so

all: $(LIBSOURCE)
	$(CC) -fPIC -c $(LIBSOURCE) -o $(LIBOBJ)
	$(CC) -shared -Wl,-soname,$(LIB) -o $(LIB) $(LIBOBJ)

clean:
	rm -rf *.so *.o
