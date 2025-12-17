CC=gcc
CFLAGS=-I.
INC=sem.h functions.h
OBJECTS=partec.o functions.o sem.o
COMPLIB=*.gch
EXECUTABLE=partec

partec: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(EXECUTABLE)

partec.o: partec.c $(INC)
	$(CC) -c partec.c $(INC) $(CFLAGS)

functions.o: functions.c $(INC)
	$(CC) -c functions.c  $(INC) $(CFLAGS)

sem.o: sem.c $(INC)
	$(CC) -c sem.c  $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(COMPLIB) $(EXECUTABLE)
