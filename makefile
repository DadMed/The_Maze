CC = gcc
CFLAGS = -Wall -Wextra -pedantic
SDL_FLAGS = `sdl2-config --cflags --libs`
SRC = src/main.c
OBJ = obj/main.o
TARGET = maze

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(SDL_FLAGS)

obj/main.o: $(SRC)
	mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f obj/*.o $(TARGET)

.PHONY: all clean

