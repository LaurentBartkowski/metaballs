OBJECTS=  mathlib.o textures.o objets.o main.o Vecteur3d.o champ.o Metaball.o 

CC=g++

all: metaballs

main.o:
	$(CC) -I/usr/local/include/SDL -DREENTRANT -DHAVE_OPENGL -c -o main.o main.cpp

%.o: %.c
	$(CC) -Os -Wall -ansi -pedantic -ffloat-store -c -g -o $@ $<

clean:
	rm -f *.o;rm metaballs

metaballs:  $(OBJECTS)
	$(CC) -g -o metaballs $(OBJECTS) -L/usr/X11R6/lib -L/usr/local/lib -lSDL -lstdc++ -lm -lGL -lGLU -lXxf86vm -lpthread -lSDL_image
