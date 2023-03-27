# A make file that should work on most Unix-like platforms

LIBP = lib
LIB1 = rxmpfr

REXX_CFLAGS := $(shell oorexx-config  --cflags)
MPFR_CFLAGS := $(shell pkgconf mpfr   --cflags)

OTHR_CFLAGS  = -DHAVE_STDBOOL_H -DHAVE_STDINT_H -fPIC

REXX_LFLAGS := $(shell oorexx-config --libs)
REXX_RPATH  := $(shell oorexx-config --rpath)

MPFR_LFLAGS := $(shell pkgconf mpfr  --libs)

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
	EXT = dylib
	OTHR_LFLAGS := -dynamiclib
else
  EXT = so
  OTHR_LFLAGS := -shared -export-dynamic -nostartfiles
endif

# What we want to build.
all: lib$(LIB1).$(EXT)

$(LIB1).o: $(LIB1).cpp
	$(CC) -c $(LIB1).cpp $(REXX_CFLAGS) $(MPFR_CFLAGS) $(OTHR_CFLAGS) -o $(LIB1).o

$(LIBP)$(LIB1).$(EXT): $(LIB1).o
	$(CC) $(LIB1).o $(REXX_LFLAGS) $(MPFR_LFLAGS) $(OTHR_LFLAGS) -o $(LIBP)$(LIB1).$(EXT)

clean:
	rm -f *.o  *.so *.dylib
