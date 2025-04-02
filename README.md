# Snake

<p>This is a basic Snake game coded mainly in C and mainly uses the SDL3 library. My SDL3 version of my Snake game is a lot better than my SDL2 version. No severe graphical bugs as far as I can tell. Downloading SDL3 is not required to run this program as the entire library is already inside this project.</p>

<h2>How to Set Up</h2>

<p>To access Snake, you will need to download the entire repository in a zip folder and unzip it on your computer. This project will contain the entire SDL3 library and all source files for the game including the font and source code in which you can change to your liking. If you want to play Snake on Windows, all you need is the terminal. If you want to play Snake on Linux, you need your terminal and Wine. If you don't have access to Wine you could translate the batch script into a makefile and build that way. I'm not sure how Macs work as I'm writing this, so I'll ignore them for now. In your terminal, change the terminal directory to the project folder. If you're on Windows, run "./build.bat". If you're on Linux, run "wine cmd < build.bat" (not sure if this command works 100%; if you're reading this, I haven't tested it yet). After running, the game should automatically launch but if it doesn't you can always launch it manually. If you already have SDL3 installed on your computer and you don't want to waste storage, edit the source code and batch script to instead use your computer's SDL3 files.</p>

<h2>How to Play</h2>

<p>Use the WASD keyes to move the green snake around. Go to the apple (red square) to increase your snake's length. The apple spawns in a new position. Press P to pause the game. You game over if you collide with yourself or leave the window. You will need to close the window and relaunch it to play again.</p>

<h2>Bugs</h2>

<p>The apple has the possibility to spawn inside the player's body. Late in the game it would make getting the apple near impossible. The pause and game over text are not centeyou exit the red. Keyboard input can be unresponsive at some points (this is technically SDL3's fault). While moving in a direction, if you then move in a different direction and then move the direction opposite of the first direction you were facing in quick succession, you collide with yourself, dying (you can't die from this when the snake length is 1).</p>

<h2>Other Information</h2>

<p>This is my first time making something in C, so no surprise there are bugs. I tried adding as many informational comments in my code as I could without it being too much so you could fix the bugs yourself. I'll add more features over time, like speed powerups, a better pause/game over menu, savable scores, more customizability, etc.</p>
