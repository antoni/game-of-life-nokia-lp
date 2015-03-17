# Conway's Game of Life for Nokia Lights Player
An entry submitted to Wrocławska Zorza Polarna (en. Aurora of Wrocław) competition. It is a C implementation of [Conway's Game of Life](http://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) created to be shown on a 16x8 pixel  display on a Nokia office building. 
### How it looks like
[![Nokia Lights Player video](http://img.youtube.com/vi/_g1BPFE-b6s/0.jpg)](http://www.youtube.com/watch?v=_g1BPFE-b6s)
### How it works
The game loads a starting configuration (modified [Glider pattern](http://en.wikipedia.org/wiki/Glider_%28Conway%27s_Life%29)) 

When the game ends (i.e. all cells die), a screen with different color for each row is being shown, then the next configuration is loaded and the game starts again.

The program itself generates a `.ile` animation file that could be loaded to a Nokia Lights Player or onto a display driver.

See `animate.c` for more info.
### Compilation 
Use MinGW with `-std=c99` flag.

