# fallblock

## Ubuntu/Linux Dependency Install
```bash
# install SDL dependencies
apt install libsdl2-dev
apt install libsdl2-image-dev
apt install libsdl2-ttf-dev
apt install libsdl2-mixer-dev
```

## MacOs Dependency Install
```bash
# install sdl2 dependencies
brew install sdl2
brew install sdl2_image
brew install sdl2_ttf
```

## Building
```bash
./build.sh # run cmake to build the target
./build/fallblock # run the game
```

Valgrind isn't supported on the latest MacOS version I'm running, so the `leaks` target won't function as expected.
