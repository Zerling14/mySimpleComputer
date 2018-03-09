COMP_NAME = mySimpleComputer
ASMC_NAME = sat

COMP_PATH = computer
ASMC_PATH = asmc

SRC_PATH = src
BUILD_PATH = build
LIB_PATH = lib

OPT_LIB = -lm

SRC_EXT = c

COMPILE_FLAGS = -Wall -Werror

SOURCES_COMP = $(shell find $(SRC_PATH)/$(COMP_PATH)/ -name '*.$(SRC_EXT)')
LIB_SOURCES_COMP = $(filter-out $(SRC_PATH)/$(COMP_PATH)/main.c, $(SOURCES_COMP))

SOURCES_ASMC = $(shell find $(SRC_PATH)/$(ASMC_PATH)/ -name '*.$(SRC_EXT)')
LIB_SOURCES_ASMC = $(filter-out $(SRC_PATH)/$(ASMC_PATH)/main.c, $(SOURCES_ASMC))

OBJECTS_COMP = $(SOURCES_COMP:$(SRC_PATH)/$(COMP_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/$(COMP_PATH)/%.o)
LIBS_COMP = $(LIB_SOURCES_COMP:$(SRC_PATH)/$(COMP_PATH)/%.$(SRC_EXT)=$(LIB_PATH)/$(COMP_PATH)/%.a)

OBJECTS_ASMC = $(SOURCES_ASMC:$(SRC_PATH)/$(ASMC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/$(ASMC_PATH)/%.o)
LIBS_ASMC = $(LIB_SOURCES_ASMC:$(SRC_PATH)/$(ASMC_PATH)/%.$(SRC_EXT)=$(LIB_PATH)/$(ASMC_PATH)/%.a)

all: dirs $(COMP_NAME) $(ASMC_NAME)
	
run: all
	./$(COMP_NAME)

$(COMP_NAME): $(BUILD_PATH)/$(COMP_PATH)/main.o $(LIBS_COMP)
	gcc $(COMPILE_FLAGS) $^ -o $(COMP_NAME) $(OPT_LIB)
	
$(ASMC_NAME): $(BUILD_PATH)/$(ASMC_PATH)/main.o $(LIBS_ASMC)
	gcc $(COMPILE_FLAGS) $^ -o $(ASMC_NAME) $(OPT_LIB)

$(LIB_PATH)/$(COMP_PATH)/%.a : $(BUILD_PATH)/$(COMP_PATH)/%.o
	ar rcs $@ $^
	
$(LIB_PATH)/$(ASMC_PATH)/%.a : $(BUILD_PATH)/$(ASMC_PATH)/%.o
	ar rcs $@ $^

$(BUILD_PATH)/$(COMP_PATH)/%.o: $(SRC_PATH)/$(COMP_PATH)/%.$(SRC_EXT)
	gcc $(COMPILE_FLAGS) -c -o $@ $<
	
$(BUILD_PATH)/$(ASMC_PATH)/%.o: $(SRC_PATH)/$(ASMC_PATH)/%.$(SRC_EXT)
	gcc $(COMPILE_FLAGS) -c -o $@ $<

.PHONY: dirs
dirs:
	@mkdir -p $(dir $(OBJECTS_COMP))
	@mkdir -p $(dir $(LIBS_COMP))
	@mkdir -p $(dir $(OBJECTS_ASMC))
	@mkdir -p $(dir $(LIBS_ASMC))

.PHONY: clean
clean:
	$(RM) -r build
	$(RM) -r lib
	$(RM) $(COMP_NAME)
	$(RM) $(ASMC_NAME)
	$(RM) termsettings
