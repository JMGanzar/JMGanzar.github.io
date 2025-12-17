CC=gcc
CFLAGS=-I.
INC=func.h sem.h
OBJECTS=rps.o func.o sem.o
COMPLIB=*.gch
EXECUTABLE=rps

rps: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(EXECUTABLE)

rps.o: procesar.c func.h
	$(CC) -c procesar.c -o rps.o $(CFLAGS)

func.o: func.c func.h
	$(CC) -c func.c  $(CFLAGS)
	
sem.o: sem.c $(INC)
	$(CC) -c sem.c  $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(COMPLIB) 
	rm $(EXECUTABLE)
	rm *vs*.dat 
