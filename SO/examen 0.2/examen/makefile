CC=gcc
CFLAGS=-I.
INC=func.h sem.h
OBJECTS=reparar.o func.o sem.o
COMPLIB=*.gch
EXECUTABLE=reparar

reparar: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(EXECUTABLE)

reparar.o: reparar.c func.h
	$(CC) -c reparar.c -o reparar.o $(CFLAGS)

func.o: func.c func.h
	$(CC) -c func.c  $(CFLAGS)

sem.o: sem.c $(INC)
	$(CC) -c sem.c  $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(COMPLIB)
	rm $(EXECUTABLE)
