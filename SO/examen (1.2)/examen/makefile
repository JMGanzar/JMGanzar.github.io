CC=gcc
CFLAGS=-I.
INC=func.h sem.h
OBJECTS=votar.o func.o sem.o
COMPLIB=*.gch
EXECUTABLE=votar

votar: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(EXECUTABLE)

votar.o: votar.c func.h
	$(CC) -c votar.c -o votar.o $(CFLAGS)

func.o: func.c func.h
	$(CC) -c func.c  $(CFLAGS)

sem.o: sem.c $(INC)
	$(CC) -c sem.c  $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(COMPLIB)
	rm $(EXECUTABLE)
