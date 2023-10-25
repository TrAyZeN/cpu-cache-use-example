CC := gcc

CPPFLAGS += -Isrc
CFLAGS += -std=gnu17 -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -Wundef \
		  -Wconversion -Wsign-conversion -Wformat=2 -O3 -g
LDFLAGS +=

BINARIES := list vector

.PHONY: all clean fmt

all: $(BINARIES)

list: src/list.c src/utils.c

vector: src/vector.c src/utils.c

$(BINARIES):
	$(CC) $^ -o $@ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) $(BINARIES)

fmt:
	clang-format --style=file -i src/**.[ch]
