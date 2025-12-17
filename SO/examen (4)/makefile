CC=gcc
CFLAGS=-I. -lm
INC=functions.h
OBJECTS=partec.o functions.o 
COMPLIB=*.gch
EXECUTABLE=partec

partec: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(EXECUTABLE)

partec.o: partec.c $(INC)
	$(CC) -c partec.c $(INC) $(CFLAGS)

functions.o: functions.c $(INC)
	$(CC) -c functions.c  $(INC) $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(COMPLIB) $(EXECUTABLE)
