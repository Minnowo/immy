
_CC    := gcc
CC     ?= gcc
CFLAGS ?= -Wall -Wextra -std=c99

TARGET_DIR ?= .
TARGET     ?= "$(TARGET_DIR)/doko"

OBJS := main.o doko.o ui.o input.o resources.o external/strnatcmp.o $(TARGET_DIR)/bundler.o

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
	rm -f $(TARGET).exe

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

$(TARGET_DIR)/bundler.o: bundler.c | $(TARGET_DIR)
	$(_CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(INCLUDE_PATHS)
	$(TARGET_DIR)/bundler.o

doko.o: doko.c doko.h darray.h | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATHS)

main.o: main.c doko.h darray.h ui.h config.h input.h | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATHS)

resources.o: resources.c resources.h | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATHS)

ui.o: ui.c doko.h ui.h config.h resources.h | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATHS)

input.o: input.c doko.h ui.h config.h | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATHS)

external/strnatcmp.o: external/strnatcmp.c | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATHS)

$(TARGET): doko.o ui.o main.o input.o resources.o external/strnatcmp.o | $(TARGET_DIR) $(TARGET_DIR)/bundler.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(INCLUDE_PATHS) $(LDFLAGS)

