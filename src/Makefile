# 05may10jir
# (c) Software Lab. Alexander Burger

lib = ../lib

picoFiles = plio.c

CFLAGS := -c -O2 -m32 -pipe \
	-falign-functions -fomit-frame-pointer -fno-strict-aliasing \
	-W -Wimplicit -Wreturn-type -Wunused -Wformat \
	-Wuninitialized -Wstrict-prototypes \
	-D_GNU_SOURCE  -D_FILE_OFFSET_BITS=64

ifeq ($(shell uname), Linux)
	OS = Linux
	PICOLISP-FLAGS = -m32 -rdynamic
	LIB-FLAGS = -lc -lm -ldl
	DYNAMIC-LIB-FLAGS = -m32 -shared -export-dynamic
	STRIP = echo #strip
else
ifeq ($(shell uname), OpenBSD)
	OS = OpenBSD
	PICOLISP-FLAGS = -m32 -rdynamic
	LIB-FLAGS = -lc -lm
	DYNAMIC-LIB-FLAGS = -m32 -shared -export-dynamic
	STRIP = strip
else
ifeq ($(shell uname), FreeBSD)
	OS = FreeBSD
	PICOLISP-FLAGS = -m32 -rdynamic
	LIB-FLAGS = -lc -lm
	DYNAMIC-LIB-FLAGS = -m32 -shared -export-dynamic
	STRIP = strip
else
ifeq ($(shell uname), NetBSD)
	OS = NetBSD
	PICOLISP-FLAGS = -m32 -rdynamic
	LIB-FLAGS = -lc -lm
	DYNAMIC-LIB-FLAGS = -m32 -shared -export-dynamic
	STRIP = strip
else
ifeq ($(shell uname), Darwin)
	OS = Darwin
	PICOLISP-FLAGS = -m32
	LIB-FLAGS = -lc -lm -ldl
	DYNAMIC-LIB-FLAGS = -m32 -dynamiclib -undefined dynamic_lookup
	STRIP = :
else
ifeq ($(shell uname -o), Cygwin)
	OS = Cygwin
	DYNAMIC-LIB-FLAGS = -shared
	PICOLISP-FLAGS =
	DLL-DEFS = $(bin)/picolisp.dll
	STRIP = strip
	exe = .exe
	dll = .dll
endif
endif
endif
endif
endif
endif

libplio: $(lib)/libplio$(dll)

all: libplio

.c.o:
	gcc $(CFLAGS) -D_OS='"$(OS)"' $*.c

$(picoFiles:.c=.o) plio.o: 

$(lib)/libplio$(dll): plio.o
	gcc -o $(lib)/libplio$(dll) $(DYNAMIC-LIB-FLAGS) plio.o $(DLL-DEFS)
	$(STRIP) $(lib)/libplio$(dll)

# Clean up
clean:
	rm -f *.o