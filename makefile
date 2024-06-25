CC = gcc
CFLAGS = -Wall -Wextra -pedantic
SDL_FLAGS = `sdl2-config --cflags --libs`
LDLIBS = -lSDL2 -lSDL2_image -lm  # Link against SDL2 and SDL2_image libraries
SRC = src/main.c
OBJ = $(SRC:.c=.o)
TARGET = maze

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(SDL_FLAGS) $(LDLIBS)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

