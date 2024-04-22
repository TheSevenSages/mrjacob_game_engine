# Makefile
# Build rules for EECS 498: Game Engine Architecture

# Compile Engine
game_engine_linux:
	clang++	-O3	-std=c++17	src/Lua/*.c	src/Engine/*.cpp	src/Box2D/collision/*.cpp	src/Box2D/common/*.cpp	src/Box2D/dynamics/*.cpp	src/Box2D/rope/*.cpp	*.cpp	-llua5.4	-I./Engine	-I./	-I./glm	-I./rapidjson	-I./Lua	-I./LuaBridge	-I./LuaBridge/details	-I./Box2D/	-I./Box2D/dynamics/	-I./SDL2	-I./SDL_image	-I./SDL_mixer	-I./SDL_ttf	-L./lib	-lSDL2	-lSDL2_image	-lSDL2_mixer	-lSDL2_ttf	-Wno-deprecated-declarations	-Wdeprecated	-o	game_engine_linux


# Remove anything created by a makefile
clean:
	rm -f	*.o	game_engine_linux

# Syncs to my CAEN
sync:
	rsync \
	-rtv \
	--delete \
	--exclude '.git*' \
	--filter=':- .gitignore' \
	../game_engine_mrjacob/ \
	mrjacob@login.engin.umich.edu:game_engine_copy/
