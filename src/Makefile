
CC     ?= gcc
CFLAGS ?= -Wall -Wextra -std=c99

TARGET_DIR ?= .
TARGET     ?= "$(TARGET_DIR)/doko"

OBJS := "${TARGET_DIR}/main.o" \
	    "${TARGET_DIR}/doko.o" \
	    "${TARGET_DIR}/ui.o" \
	    "${TARGET_DIR}/keybind.o"

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
	rm -f $(TARGET).exe

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

doko.o: doko.c doko.h darray.h | $(TARGET_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

main.o: main.c doko.h darray.h ui.h config.h keybind.h | $(TARGET_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

ui.o: ui.c doko.h ui.h config.h | $(TARGET_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

keybind.o: keybind.c doko.h ui.h config.h | $(TARGET_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): doko.o ui.o main.o keybind.o | $(TARGET_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS)