# fallblock

## Linux Setup
```bash
// install SDL dependencies
apt install libsdl2-dev
apt install libsdl2-image-dev
apt install libsdl2-ttf-dev
apt install libsdl2-mixer-dev
```

## MacOs Setup
```bash
// install sdl2 dependencies
brew install sdl2
brew install sdl2_image
brew install sdl2_ttf
```

```bash
// symlink sdl2 dependencies to include folder
ln -s /usr/local/Cellar/sdl2/2.24.1/include/SDL2 ./include/SDL2
ln -s /usr/local/Cellar/sdl2_image/2.6.2/include/SDL2 ./include/SDL2_image
ln -s /usr/local/Cellar/sdl2_ttf/2.20.1/include/SDL2 ./include/SDL2_ttf
ln -s /usr/local/Cellar/sdl2_mixer/2.6.2/include/SDL2 ./include/SDL2_mixer

// symlink sdl2 dependencies to lib folder
ln -s /usr/local/Cellar/sdl2_image/2.6.2/lib ./lib/SDL2_image
ln -s /usr/local/Cellar/sdl2_ttf/2.20.1/lib ./lib/SDL2_ttf
ln -s /usr/local/Cellar/sdl2_mixer/2.6.2/lib ./lib/SDL2_mixer
```

Valgrind isn't supported on the latest MacOS version I'm running, so the `leaks` target won't function as expected.