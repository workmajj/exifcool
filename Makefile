NAME=exifcool

CC=clang
CFLAGS=-Wall -g
LIBS=-lexif

all: clean exifcool

exifcool:
	$(CC) -o $(NAME) $(LIBS) $(NAME).c

.PHONY: clean
clean:
	rm -f $(NAME)
	rm -rf $(NAME).dSYM
