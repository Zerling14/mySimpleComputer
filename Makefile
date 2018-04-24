COMP_NAME = mySimpleComputer
ASMC_NAME = sat
BASIC_NAME = basic

COMP_PATH = computer
ASMC_PATH = asmc
BASIC_PATH = basic

SRC_PATH = src
BUILD_PATH = build
LIB_PATH = lib

OPT_LIB_PATH = -L./lib

OPT_INCLUDE_PATH = -I./src/computer

OPT_LIB = -lm -lmemory -lcommand -lcpu -ldebug -linterface -lmyreadkey -lmyterm -lbigchars

OPT_LIB_ASMC = -lm -llexer -lprintinfo -lmemory -lcommand

OPT_LIB_BASIC = -lm -llexer -lprintinfo -lmemory -lcommand

SRC_EXT = c

COMPILE_FLAGS = -Wall -Werror

SOURCES_COMP = $(shell find $(SRC_PATH)/$(COMP_PATH)/ -name '*.$(SRC_EXT)')
LIB_SOURCES_COMP = $(filter-out $(SRC_PATH)/$(COMP_PATH)/main.c, $(SOURCES_COMP))

SOURCES_ASMC = $(shell find $(SRC_PATH)/$(ASMC_PATH)/ -name '*.$(SRC_EXT)')
LIB_SOURCES_ASMC = $(filter-out $(SRC_PATH)/$(ASMC_PATH)/main.c, $(SOURCES_ASMC))

SOURCES_BASIC = $(shell find $(SRC_PATH)/$(BASIC_PATH)/ -name '*.$(SRC_EXT)')
LIB_SOURCES_BASIC = $(filter-out $(SRC_PATH)/$(BASIC_PATH)/main.c, $(SOURCES_BASIC))

OBJECTS_COMP = $(SOURCES_COMP:$(SRC_PATH)/$(COMP_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/$(COMP_PATH)/%.o)
LIBS_COMP = $(LIB_SOURCES_COMP:$(SRC_PATH)/$(COMP_PATH)/%.$(SRC_EXT)=$(LIB_PATH)/lib%.a)

OBJECTS_ASMC = $(SOURCES_ASMC:$(SRC_PATH)/$(ASMC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/$(ASMC_PATH)/%.o)
LIBS_ASMC = $(LIB_SOURCES_ASMC:$(SRC_PATH)/$(ASMC_PATH)/%.$(SRC_EXT)=$(LIB_PATH)/lib%.a)

OBJECTS_BASIC = $(SOURCES_BASIC:$(SRC_PATH)/$(BASIC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/$(BASIC_PATH)/%.o)
LIBS_BASIC = $(LIB_SOURCES_BASIC:$(SRC_PATH)/$(BASIC_PATH)/%.$(SRC_EXT)=$(LIB_PATH)/lib%.a)

all: dirs $(COMP_NAME) $(ASMC_NAME) $(BASIC_NAME)
	
run: all
	./$(COMP_NAME)

$(COMP_NAME): $(BUILD_PATH)/$(COMP_PATH)/main.o $(LIBS_COMP)
	gcc $(COMPILE_FLAGS) $< -o $(COMP_NAME) $(OPT_LIB_PATH) $(OPT_LIB)
	
$(ASMC_NAME): $(BUILD_PATH)/$(ASMC_PATH)/main.o $(LIBS_ASMC)
	gcc $(COMPILE_FLAGS) $< -o $(ASMC_NAME) $(OPT_INCLUDE_PATH) $(OPT_LIB_PATH) $(OPT_LIB_ASMC)
	
$(BASIC_NAME): $(BUILD_PATH)/$(BASIC_PATH)/main.o $(LIBS_BASIC)
	gcc $(COMPILE_FLAGS) $< -o $(BASIC_NAME) $(OPT_INCLUDE_PATH) $(OPT_LIB_PATH) $(OPT_LIB_BASIC)

$(LIB_PATH)/lib%.a : $(BUILD_PATH)/$(COMP_PATH)/%.o
	ar rcs $@ $^
	
$(LIB_PATH)/lib%.a : $(BUILD_PATH)/$(ASMC_PATH)/%.o
	ar rcs $@ $^
	
$(LIB_PATH)/lib%.a : $(BUILD_PATH)/$(BASIC_PATH)/%.o
	ar rcs $@ $^

$(BUILD_PATH)/$(COMP_PATH)/%.o: $(SRC_PATH)/$(COMP_PATH)/%.$(SRC_EXT)
	gcc $(COMPILE_FLAGS) -c -o $@ $< $(OPT_INCLUDE_PATH)
	
$(BUILD_PATH)/$(ASMC_PATH)/%.o: $(SRC_PATH)/$(ASMC_PATH)/%.$(SRC_EXT)
	gcc $(COMPILE_FLAGS) -c -o $@ $< $(OPT_INCLUDE_PATH)

$(BUILD_PATH)/$(BASIC_PATH)/%.o: $(SRC_PATH)/$(BASIC_PATH)/%.$(SRC_EXT)
	gcc $(COMPILE_FLAGS) -c -o $@ $< $(OPT_INCLUDE_PATH)

.PHONY: dirs
dirs:
	@mkdir -p $(dir $(OBJECTS_COMP))
	@mkdir -p $(dir $(LIBS_COMP))
	@mkdir -p $(dir $(OBJECTS_ASMC))
	@mkdir -p $(dir $(LIBS_ASMC))
	@mkdir -p $(dir $(OBJECTS_BASIC))
	@mkdir -p $(dir $(LIBS_BASIC))

.PHONY: clean
clean:
	$(RM) -r build
	$(RM) -r lib
	$(RM) $(COMP_NAME)
	$(RM) $(ASMC_NAME)
	$(RM) $(BASIC_NAME)
	$(RM) termsettings
