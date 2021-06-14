# Tuidoku

Simple ncurses program to play sudoku in the terminal
written entirely in C++

## How to Play
Move around the board and insert numbers or pencil-marks
according to the keys set in ```config.h```.

## How to configure
Configuration is done in the config.h file. Documentation for 
the configuration is in the form of comments in that same file.
All configuration must be done before installation.
Changing any configuration will require reinstallation.

## How to Install
clone this repository then compile using g++ or clang++    

### Linux
```
git clone https://github.com/flyingpeakock/tuidoku.git
cd tuidoku/
g++ -O3 -pthread ./src/*.cpp -lncursesw -o tuidoku
```
### OSX
```
git clone https://github.com/flyingpeakock/tuidoku.git
cd tuidoku/
clang++ -O3 -pthread -std=c++11 -stdlib=libc++ ./src/*.cpp -lncursesw -o tuidoku
```

-lncursesw flag is required to link ncurses.h   
-O3 is recommended to speed up puzzle generation but is not required.   

To run this program from any directory move the generated
tuidoku file to anywhere in your $PATH.
```
ln -s tuidoku ~/.local/bin/tuidoku
```

### Screenshots
![new game](https://i.imgur.com/CdTClft.png)
![same game mid playing](https://i.imgur.com/Y3NT2Tb.png)
![same game state with check](https://i.imgur.com/dRXx3j0.png)

### Troubleshooting
If the compiler cannot find ncurses.h you need to make sure that ncurses is installed.
If you cannot find ncurses in your package manager it might be called something like
libcurses or curses instead. If it still doesn't work use the -lncurses flag instead
of -lncursesw when compiling.
