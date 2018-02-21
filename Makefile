BIN_NAME = mySimpleComputer

SRC_PATH = src
BUILD_PATH = build
LIB_PATH = lib

OPT_LIB = -lm

SRC_EXT = c

COMPILE_FLAGS = -Wall -Werror

SOURCES = $(shell find $(SRC_PATH)/ -name '*.$(SRC_EXT)')
LIB_SOURCES = $(filter-out $(SRC_PATH)/main.c, $(SOURCES))

OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
LIBS = $(LIB_SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(LIB_PATH)/%.a)

all: dirs  
	@$(MAKE) release
	
run: all
	./$(BIN_NAME)

release: $(BUILD_PATH)/main.o $(LIBS)
	gcc $(COMPILE_FLAGS) $^ -o $(BIN_NAME) $(OPT_LIB)

$(BUILD_PATH)/main.o : $(SRC_PATH)/main.c
	gcc $(COMPILE_FLAGS) -c -o $@ $<

$(LIB_PATH)/%.a : $(BUILD_PATH)/%.o
	ar rcs $@ $^

$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	gcc $(COMPILE_FLAGS) -c -o $@ $<

.PHONY: dirs
dirs:
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(dir $(LIBS))

.PHONY: clean
clean:
	$(RM) -r build
	$(RM) -r lib
	$(RM) $(BIN_NAME)
