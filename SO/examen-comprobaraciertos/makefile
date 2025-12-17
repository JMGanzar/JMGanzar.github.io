CC=gcc
CFLAGS=-lm -I.
INC=sem.h 
OBJECTS=shared.o sem.o func.o
FILES=r*

shared: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o shared

shared.o: shared.c $(INC)
	$(CC) -c shared.c  $(CFLAGS)

func.o: func.c func.h $(INC)
	$(CC) -c func.c $(CFLAGS) 

sem.o: sem.c $(INC)
	$(CC) -c sem.c  $(CFLAGS)

clean:
	rm shared $(OBJECTS) $(FILES)
