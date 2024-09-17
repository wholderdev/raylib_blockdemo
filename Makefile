CC = gcc
CFLAGS = -Wall

LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC = block_demo.c
OBJ = block_demo

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -o $(OBJ) $(SRC) $(LIBS)

clean:
	rm -f $(OBJ)
