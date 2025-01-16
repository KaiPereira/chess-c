CC = gcc
CFLAGS = -I.
OBJ = main.o positions.o openings.o

main: $(OBJ)
	$(CC) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f main *.o
