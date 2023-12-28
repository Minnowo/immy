
CC     ?= gcc
CFLAGS ?= -Wall -Wextra -std=c99

TARGET_DIR ?= .
TARGET     ?= "$(TARGET_DIR)/doko"

OBJS := main.o doko.o ui.o input.o

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
	rm -f $(TARGET).exe

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

doko.o: doko.c doko.h darray.h | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATHS)

main.o: main.c doko.h darray.h ui.h config.h input.h | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATHS)

ui.o: ui.c doko.h ui.h config.h | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATHS)

input.o: input.c doko.h ui.h config.h | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATHS)

$(TARGET): doko.o ui.o main.o input.o | $(TARGET_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(INCLUDE_PATHS) $(LDFLAGS)