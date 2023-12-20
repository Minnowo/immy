
SRC_DIR := .

DOKO_OBJS := $(BUILD_DIR)/main.o $(BUILD_DIR)/ui.o $(BUILD_DIR)/doko.o

all: ${DOKO_OBJS}

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)
