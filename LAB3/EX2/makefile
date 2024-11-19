CPP = g++                    # C++ compiler
CC = gcc                     # C compiler
LD = ld                      # Linker
AR = ar                      # Archiver for static libraries
GFLAGS = -std=c++11 -g -pthread -I./ -L.   # Compilation flags for C++
CFLAGS = -g -lpthread -I./ -L.  # Compilation flags for C
LFLAGS = -g -I./ -L.           # Linking flags
ARFLAGS = rcs                 # Flags for creating static libraries

# Object files and executable
COBJECT = main                # Main source file (without extension)
CEXEC = aggsum                # Final executable
LIBS = utils                  # Library names

# Object files
COBJECTO = $(addsuffix .o,$(COBJECT))
CLIBS = $(addprefix lib,$(addsuffix .a,$(LIBS)))

# Compile .cpp files into object files
%.o: %.cpp
	$(CPP) $(GFLAGS) -c $< -o $@

# Compile .c files into object files with -fPIC and -fPIE for the library
%.o: %.c
	$(CC) $(CFLAGS) -fPIC -fPIE -c $< -o $@

# Create static library from object files (with -fPIC and -fPIE)
lib%.a: %.o
	$(AR) $(ARFLAGS) $@ $^

# Build the main executable with -fPIE (for position-independent executable)
all: $(COBJECTO) $(CLIBS)
	$(CC) $(GFLAGS) -fPIE $^ -o $(CEXEC)

# Clean up object files and executable
clean:
	rm -f *.o $(CEXEC)
