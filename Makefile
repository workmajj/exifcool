CC = clang
CFLAGS = -MD -Wall -Werror -std=c99 -I/usr/local/include
LDFLAGS = -L/usr/local/lib
LDLIBS = -lexif

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

exifcool: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o exifcool

.PHONY: clean
clean:
	rm -f exifcool
	rm -f $(OBJS) $(DEPS)
