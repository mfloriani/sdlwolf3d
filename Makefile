build:
	gcc -Wfatal-errors \
	-std=c99 \
	./*.c \
	-I"C:/libsdl/include" \
	-L"C:/libsdl/lib" \
	-lmingw32 \
	-lSDL2main \
	-lSDL2 \
	-o main.exe

run:
	main.exe

clean:
	del main.exe