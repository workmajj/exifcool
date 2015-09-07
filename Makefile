NAME=exifcool

CC=clang
CFLAGS=-Wall -g
LIBS=-lexif
INCLUDES=trie.c

all: clean exifcool

exifcool:
	$(CC) -o $(NAME) $(LIBS) $(NAME).c $(INCLUDES)

.PHONY: clean
clean:
	rm -f $(NAME)
	rm -rf $(NAME).dSYM
