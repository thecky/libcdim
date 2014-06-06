CC=g++
LIBSOURCE=libcdim.cpp
LIBOBJ=libcdim.o
LIB=libcdim.so.0.1

all: $(LIBSOURCE)
	$(CC) -fPIC -c $(LIBSOURCE) -o $(LIBOBJ)
	$(CC) -shared -Wl,-soname,$(LIB) -o $(LIB) $(LIBOBJ)
	ln -s $(LIB) libcdim.so

clean:
	rm -rf *.so.* *.o
