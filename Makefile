IDIR=include
CC=gcc
CFLAGS=-I$(IDIR) -I/usr/include/fuse3 -Wall --std=c++17 

ODIR=obj
LDIR=lib
SDIR=src

LIBS=-lm

_DEPS = fusehttp.h Fuse.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = fusehttp.o fusehttp-fs.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(OBJ): $(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

build: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 