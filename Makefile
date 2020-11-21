
# Specify project name and default compilers
OUT = cdp_test
CC = gcc
CXX = g++

######################################################################

# Get actual date and setup output binary directory name
_BINDIR = ./bin
BINDIR = bin/$(shell date '+%Y_%m_%d_%H_%M_%S')

# Specify sources and headers files
SRCS  = $(shell find ./src -type f -name *.c)
SRCS  += $(shell find ./src -type f -name *.cpp)

# Specify directory where store compile objects files
OBJDIR = ./build

# Get objects files from sources and output object
_OBJS = $(SRCS:.cpp=.o)
OBJS = $(_OBJS:.c=.o)

# Setup compilation flags
CXXFLAGS = -O0 -Wall -g $(LIBS)
# Note: Optimization set to 0 for debug in code order

######################################################################

# Target: make all (build project generating output directory)
all: $(OUT)
	mkdir -p ${OBJDIR}
	mkdir -p ${BINDIR}
	cp -a $(OUT) $(BINDIR)
	mv *.o $(OBJDIR)/

# Target: make clean (remove all previously builds)
clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(OUT)

# Target: make cleanall clean previously builds including output bins)
cleanall: clean
	rm -rf $(OBJDIR)
	rm -rf $(_BINDIR)

# Target: make rebuild (clean previously builds and build again)
rebuild: clean all

# Target: check (custom target to check build variables)
check:
	@echo "SRCS:"
	@echo "  $(SRCS)"
	@echo "OBJS:"
	@echo "  $(OBJS)"
	@echo "BINDIR:"
	@echo "  $(BINDIR)"

######################################################################

# Target: make <OUT> (build project)
$(OUT): $(OBJS) $(HEADS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

# Target for generate object file of each .c file
%.o: %.c
	$(CC) $(CXXFLAGS) -c $<

# Target for generate object file of each .cpp file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<
