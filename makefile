/** credits for makefile: **/

debug:
	g++ -c src/*.cpp -std=c++14 -g -Wall -m64 -I include && g++ *.o -o bin/debug/main -L lib -lmingw32 -lSDL2main -lSDL2 -mwindows && start bin/debug/main

release:
	g++ -c src/*.cpp -std=c++14 -O3 -Wall -m64 -I include && g++ *.o -o -i bin/release/main -L lib -lmingw32 -lSDL2main -lSDL2 -mwindows && start bin/release/main

