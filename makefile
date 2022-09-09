all :
	gcc -std=c17 processor.c main.c -ISDL2\include -LSDL2\lib -lmingw32 -lSDL2main -lSDL2 -o builds\main
	builds\main.exe
