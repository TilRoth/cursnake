all: snake

snake: snake.c
	gcc snake.c -o snake -O3 -lncurses

play:
	./snake

clean:
	rm -f snake
