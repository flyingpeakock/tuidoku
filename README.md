# Tuidoku

Simple ncurses program to play sudoku in the terminal
written entirely in C++

## How to Play
Move around the board and insert numbers or pencil-marks
according to the keys set in ```config.h```. The defualt
keys for movement are 'hjkl', 'i' for insert, 'p' for pencil and 'c'
for check. To go to a specific box press the go key, default 'g',
followed by the column then row.

## How to configure
Configuration is done in the config.h file. Documentation for 
what configuration does what can be read in the file.
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

### Troubleshooting
If the compiler cannot find ncurses.h you need to make sure that ncurses is installed.
If you cannot find ncurses in your package manager it might be called something like
libcurses or curses instead. If it still doesn't work use the -lncurses flag instead
of -lncursesw when compiling.
