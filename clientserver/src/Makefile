# Define the compiler and the linker. The linker must be defined since
# the implicit rule for linking uses CC as the linker. g++ can be
# changed to clang++.
CXX = g++
CC  = $(CXX)

# Generate dependencies in *.d files
DEPFLAGS = -MT $@ -MMD -MP -MF $*.d

# Define preprocessor, compiler, and linker flags. Uncomment the # lines
# if you use clang++ and wish to use libc++ instead of GNU's libstdc++.
# -g is for debugging.
CPPFLAGS  = -std=c++14 -I..
CXXFLAGS =  -O2 -Wall -Wextra -pedantic-errors -Wold-style-cast
CXXFLAGS += -std=c++14
CXXFLAGS += -g
CXXFLAGS += $(DEPFLAGS)
LDFLAGS   = -g -L..
#CPPFLAGS += -stdlib=libc++
#CXXFLAGS += -stdlib=libc++
#LDFLAGS +=  -stdlib=libc++

# Libraries
LDLIBS = libclientserver.a


SRC_DIR := 
BUILD_DIR := ../build/
BUILD_TARGS := $(patsubst %,$(DEST_DIR)%,$(PROGS))

# Targets
PROGS = myserver myclient

all: libclientserver.a $(PROGS)


# Targets rely on implicit rules for compiling and linking
# The dependency on libclientserver.a is not defined.
myserver: $(SRC_DIR)myserver.o  $(SRC_DIR)messagehandler.o $(SRC_DIR)newsgroup.o $(SRC_DIR)storagememory.o $(SRC_DIR)storagedisk.o 
myclient: $(SRC_DIR)myclient.o $(SRC_DIR)messagehandler.o
libclientserver.a: $(SRC_DIR)server/connection.o $(SRC_DIR)server/server.o
	ar rv $(SRC_DIR)libclientserver.a  $(SRC_DIR)server/connection.o $(SRC_DIR)server/server.o
	ranlib $(SRC_DIR)libclientserver.a

SRC = $(wildcard $(SRC_DIR)*.cc)

# Phony targets
.PHONY: all clean distclean install

# Standard clean
clean:
	rm -f $(SRC:.cc=.o) $(PROGS)


distclean: clean
	-rm $(SRC_DIR)libclientserver.a
	-rm $(SRC:.cc=.d)
	-rm -r $(addsuffix .dSym,$(PROGS)) # for cleaning on Macs

install:
	cp -f $(PROGS) $(BUILD_DIR)



# Include the *.d files
-include $(SRC:.cc=.d)
