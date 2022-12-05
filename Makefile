FILES = src/main.c src/gl_utils.c src/scene_buffer.c src/linalg.c

default: $(FILES)
	gcc -g $(FILES) -lGL -lGLEW -lglfw -lglut
	./a.out