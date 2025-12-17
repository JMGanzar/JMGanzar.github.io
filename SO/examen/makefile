CC=gcc
CFLAGS=-I.
INC=qdbmp.h func.h sem.h
OBJECTS=imagen.o func.o qdbmp.o sem.o
COMPLIB=*.gch
EXECUTABLE=imagen

imagen: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(EXECUTABLE)

imagen.o: imagen.c $(INC)
	$(CC) -c imagen.c $(CFLAGS)

func.o: func.c $(INC)
	$(CC) -c func.c  $(CFLAGS)

sem.o: sem.c $(INC)
	$(CC) -c sem.c  $(CFLAGS)

qdbmp.o: qdbmp.c $(INC)
	$(CC) -c qdbmp.c  $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(COMPLIB) 
	rm *.txt *VOLT.bmp
	