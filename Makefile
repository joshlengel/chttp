SRC_DIR := src
INCLUDE_DIR := include
OBJ_DIR := obj
LIB_DIR := lib

DEBUG_DIR := debug
RELEASE_DIR := release

TARGET_NAME := libchttp.so

CFLAGS += -Wall -Wextra -Wpedantic -fPIC -I$(INCLUDE_DIR)
LDFLAGS += -shared -lpthread

sources := $(wildcard $(SRC_DIR)/*.c)
debug_objs := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/$(DEBUG_DIR)/%.o, $(sources))
release_objs := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/$(RELEASE_DIR)/%.o, $(sources))

PREFIX := /usr/local

.PHONY: clean debug release example

debug: CFLAGS += -g
debug: $(LIB_DIR)/$(DEBUG_DIR)/$(TARGET_NAME)

release: CFLAGS += -O2 -DNDEBUG
release: $(LIB_DIR)/$(RELEASE_DIR)/$(TARGET_NAME)

example: release
	cd example && $(MAKE)

clean:
	cd example && $(MAKE) clean
	rm -rf lib/ obj/
	mkdir -p $(LIB_DIR)/$(DEBUG_DIR) $(LIB_DIR)/$(RELEASE_DIR) $(OBJ_DIR)/$(DEBUG_DIR) $(OBJ_DIR)/$(RELEASE_DIR)

install:
	mkdir -p $(DESTDIR)$(PREFIX)/include $(DESTDIR)$(PREFIX)/lib
	cp -r $(INCLUDE_DIR)/* $(DESTDIR)$(PREFIX)/include/
	cp $(LIB_DIR)/$(RELEASE_DIR)/$(TARGET_NAME) $(DESTDIR)$(PREFIX)/lib
	ldconfig $(DESTDIR)$(PREFIX)/lib

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/include/chttp/ $(DESTDIR)$(PREFIX)/lib/$(TARGET_NAME)

$(OBJ_DIR)/$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(OBJ_DIR)/$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(LIB_DIR)/$(DEBUG_DIR)/$(TARGET_NAME): $(debug_objs)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(LIB_DIR)/$(RELEASE_DIR)/$(TARGET_NAME): $(release_objs)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)