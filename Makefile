CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11
DEBUG_FLAGS = -g

OBJ = main.o gestion_produit.o gestion_db.o utils.o

EXEC = bgrs

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

main.o: main.c gestion_produit.h gestion_db.h utils.h
	$(CC) $(CFLAGS) -c main.c

gestion_produit.o: gestion_produit.c gestion_produit.h
	$(CC) $(CFLAGS) -c gestion_produit.c

gestion_db.o: gestion_db.c gestion_db.h gestion_produit.h
	$(CC) $(CFLAGS) -c gestion_db.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o $(EXEC)
