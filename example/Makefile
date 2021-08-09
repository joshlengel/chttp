CFLAGS += -g -Wall -Wextra -Wpedantic -fPIC -I../include
LDFLAGS += -L../lib/release -lchttp '-Wl,-R$$ORIGIN'

sources := $(wildcard src/*.c)
objs := $(patsubst src/%.c, obj/%.o, $(sources))

.PHONY: clean

../bin/example.out: $(objs)
	cp ../lib/release/libchttp.so ../bin/
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -rf ../bin/ obj/
	mkdir -p ../bin obj