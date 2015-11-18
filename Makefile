NAME=exifcool

CC=clang
CFLAGS=-Wall -g
LIBS=-lexif
INCLUDES=digittrie.c

all: clean exifcool

exifcool:
	$(CC) -I/usr/local/include -L/usr/local/lib -o $(NAME) $(LIBS) src/$(NAME).c src/$(INCLUDES)

.PHONY: clean
clean:
	rm -f $(NAME)
	rm -rf $(NAME).dSYM
