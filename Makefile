FLAG := -Wall -Wextra -Werror -Wpedantic 
all: 
	gcc $(FLAG) main.c -o bgrs



clean:
	rm -f bgrs
	


