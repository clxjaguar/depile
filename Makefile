CFLAGS = -Wall
.PHONY : all rebuild clean mrproper
all: depile
rebuild: mrproper all

depile: depile.o
	@echo "*** Linking all main objects files..."
	@gcc -lrt depile.o -o depile

depile.o: depile.c
	@echo "*** Compiling depile.o"
	@gcc $(CFLAGS) -c depile.c -o depile.o

clean:
	@echo "*** Erasing objects files..."
	@rm -f *.o

mrproper: clean
	@echo "*** Erasing executables..."
	@rm -f depile

