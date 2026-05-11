CCOMP=gcc

CCOMP_FLAGS=-std=c11 -Wall -pedantic
CLINK_FLAGS=-Wall

COMMON_LIBS=-lm -lsnaken
GRAPHICS_LIBS=-lraylib -lGL -lX11
BHM_LIBS=-lbehema

BHM_SNAKE_LIBS=

ifeq ($(GRAPHICS),1)
	BHM_SNAKE_LIBS=$(GRAPHICS_LIBS) $(COMMON_LIBS) $(BHM_LIBS)
else
	BHM_SNAKE_LIBS=$(COMMON_LIBS) $(BHM_LIBS)
endif

# Compilation mode:
# 'release' -> heavy optimizations (-O3) and no debugging.
# 'debug' -> no extra optimizations and debugging enabled.
COMPILE_MODE=debug

ifeq ($(COMPILE_MODE), debug)
    CCOMP_FLAGS+= -g
endif
ifeq ($(COMPILE_MODE), release)
    CCOMP_FLAGS+= -O3
endif

SRC_DIR=./src
BLD_DIR=./bld
BIN_DIR=./bin
IN_DIR=./in
OUT_DIR=./out

# Adds BLD_DIR to object parameter names.
OBJS=$(patsubst %.o,$(BLD_DIR)/%.o,$@.o)

MKDIR=mkdir -p
RM=rm -rf

# Check what the current operating system is.
UNAME_S=$(shell uname -s)

# The curren OS is Linux.
ifeq ($(UNAME_S),Linux)
	COMMON_LIBS+=-lrt
endif

# The current OS is MacOS.
ifeq ($(UNAME_S),Darwin)
    CCOMP_FLAGS+=-I/usr/local/include

    # Make sure libraries are found during linking (-L) and embedded in the binary (-rpath).
    CLINK_FLAGS+=-L/usr/local/lib -Wl,-rpath,/usr/local/lib
endif

all: clean snake bhm_snake

snake: create
	@printf "\n"
	$(CCOMP) $(CCOMP_FLAGS) -c $(SRC_DIR)/$@.c -o $(BLD_DIR)/$@.o
	$(CCOMP) $(CLINK_FLAGS) $(OBJS) -o $(BIN_DIR)/$@ $(GRAPHICS_LIBS) $(COMMON_LIBS)
	@printf "\nCreated $@!\n"

bhm_snake: create
	@printf "\n"
	$(CCOMP) $(CCOMP_FLAGS) -c $(SRC_DIR)/$@.c -o $(BLD_DIR)/$@.o
	$(CCOMP) $(CLINK_FLAGS) $(OBJS) -o $(BIN_DIR)/$@ $(BHM_SNAKE_LIBS)
	@printf "\nCreated $@!\n"

create:
	$(MKDIR) $(BLD_DIR)
	$(MKDIR) $(BIN_DIR)
	$(MKDIR) $(IN_DIR)
	$(MKDIR) $(OUT_DIR)

clean:
	$(RM) $(BLD_DIR)
	$(RM) $(BIN_DIR)
	$(RM) $(IN_DIR)
	$(RM) $(OUT_DIR)
