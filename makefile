# Executable
EXE    = ymert

# Compiler, Linker Defines
CC      = g++
CFLAGS  = -g -Wall -O -Wno-deprecated -I./include -I/home/asr/yuheng/bin/boost/include
LIBDIR  = ./lib64
ASM     = -lfsrilm -lz
# Compile and Assemble C++ Source Files into Object Files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
# Source and Object files
SRC    = $(wildcard *.cpp)
OBJ    = $(patsubst %.cpp, %.o, $(SRC))
	
# Link all Object Files with external Libraries into Binaries

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXE)

.PHONY: clean
clean:
	 -rm -f core *.o

