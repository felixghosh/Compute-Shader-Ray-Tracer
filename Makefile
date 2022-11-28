FILES = src/main.c src/gl_utils.c

default: $(FILES)
	gcc -g $(FILES) -lGL -lGLEW -lglfw -lglut
	./a.out