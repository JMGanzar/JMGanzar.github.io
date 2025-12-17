CC=gcc
CFLAGS=-I.
INC=qdbmp.h func.h
OBJECTS=imagen.o func.o qdbmp.o
COMPLIB=*.gch
EXECUTABLE=imagen

imagen: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(EXECUTABLE)

imagen.o: imagen.c func.h
	$(CC) -c imagen.c $(CFLAGS)

func.o: func.c func.h
	$(CC) -c func.c  $(CFLAGS)

qdbmp.o: qdbmp.c qdbmp.h
	$(CC) -c qdbmp.c  $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(COMPLIB) 
	rm R*.dat
