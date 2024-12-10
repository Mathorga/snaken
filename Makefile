CCOMP=gcc
ARC=ar

STD_CCOMP_FLAGS=-std=c17 -Wall -pedantic -g -fPIC
CCOMP_FLAGS=$(STD_CCOMP_FLAGS) -fopenmp
CLINK_FLAGS=-Wall -fopenmp
ARC_FLAGS=-rcs

# Mode flag: if set to "archive", installs snaken as a static library.
MODE=

STD_LIBS=-lrt -lm

SRC_DIR=./src
BLD_DIR=./bld
BIN_DIR=./bin

SYSTEM_INCLUDE_DIR=
SYSTEM_LIB_DIR=

# Adds BLD_DIR to object parameter names.
OBJS=$(patsubst %.o,$(BLD_DIR)/%.o,$^)

MKDIR=mkdir -p
RM=rm -rf

# Check what the current operating system is.
UNAME_S=$(shell uname -s)

# The curren OS is Linux.
ifeq ($(UNAME_S),Linux)
	SYSTEM_INCLUDE_DIR=/usr/include
	SYSTEM_LIB_DIR=/usr/lib
endif

# The current OS is MacOS.
ifeq ($(UNAME_S),Darwin)
	SYSTEM_INCLUDE_DIR=/usr/local/include
	SYSTEM_LIB_DIR=/usr/local/lib
endif

all: std

# Installs all header files to the default include dir.
install-headers:
	@printf "\nInstalling headers...\n\n"
	sudo $(MKDIR) $(SYSTEM_INCLUDE_DIR)/snaken
	sudo cp $(SRC_DIR)/*.h $(SYSTEM_INCLUDE_DIR)/snaken


# Installs the generated lib file to the default lib dir.
install-lib:
ifneq ($(MODE), archive)
	@printf "\nInstalling dynamic library...\n\n"
	sudo cp $(BLD_DIR)/libsnaken.so $(SYSTEM_LIB_DIR)
endif
ifeq ($(MODE), archive)
	@printf "\nInstalling static library...\n\n"
	sudo cp $(BLD_DIR)/libsnaken.a $(SYSTEM_LIB_DIR)
endif


# Installs the library files (headers and compiled) into the default system lookup folders.
install: std install-headers install-lib
	@printf "\nInstallation complete!\n\n"


# Uninstalls any previous installation.
uninstall: clean
	sudo $(RM) $(SYSTEM_INCLUDE_DIR)/snaken
	sudo $(RM) $(SYSTEM_LIB_DIR)/libsnaken.so
	sudo $(RM) $(SYSTEM_LIB_DIR)/libsnaken.a
	@printf "\nSuccessfully uninstalled.\n\n"

std: create build

# Builds all library files.
build: snaken.o utils.o graphics.o
	$(CCOMP) $(CLINK_FLAGS) -shared $(OBJS) $(STD_LIBS) -lraylib -o $(BLD_DIR)/libsnaken.so
	$(ARC) $(ARC_FLAGS) $(BLD_DIR)/libsnaken.a $(OBJS)
	@printf "\nCompiled $@!\n"


# Builds object files from source.
%.o: $(SRC_DIR)/%.c
	$(CCOMP) $(CCOMP_FLAGS) -c $^ -o $(BLD_DIR)/$@

%.o: $(SRC_DIR)/%.cu
	$(NVCOMP) $(NVCOMP_FLAGS) -c $^ -o $(BLD_DIR)/$@


# Creates temporary working directories.
create:
	$(MKDIR) $(BLD_DIR)
	$(MKDIR) $(BIN_DIR)

# Removes temporary working directories.
clean:
	$(RM) $(BLD_DIR)
	$(RM) $(BIN_DIR)
