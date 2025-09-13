CCOMP=gcc

STD_CCOMP_FLAGS=-std=c11 -Wall -pedantic -g
CCOMP_FLAGS=$(STD_CCOMP_FLAGS)
CLINK_FLAGS=-Wall

COMMON_LIBS=-lm -lpthread -ldl -lsnaken
GRAPHICS_LIBS=-lraylib -lGL -lX11
BHM_LIBS=-lbehema

BHM_SNAKE_LIBS=

ifeq ($(GRAPHICS),1)
	BHM_SNAKE_LIBS=$(GRAPHICS_LIBS) $(COMMON_LIBS) $(BHM_LIBS)
else
	BHM_SNAKE_LIBS=$(COMMON_LIBS) $(BHM_LIBS)
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

CCOMP_FLAGS+=-I/usr/local/include
CLINK_FLAGS+=-L/usr/local/lib

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
