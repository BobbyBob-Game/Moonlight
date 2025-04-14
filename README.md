# Moonlight - Phạm Gia Bách (24021382)
Moonlight is a 2D platformer game inspired from Celeste, Animal Well, and Dead Cells.
##
My learning source, and inspiration are primary Avery and Mike Shaw on youtube, their youtube links are:

https://www.youtube.com/playlist?list=PLVotA8ycjnCs3DNWIbEIpBrjYkhJq11q- (player animation)

https://www.youtube.com/playlist?list=PLvv0ScY6vfd-p1gSnbQhY7vMe2rng0IL0 (background parallax, text rendering)
The animation sprite, sound and tile set are free source on the internet.
##
##
The repo has 3 directories (Learning Days, Proto1, and Proto2), which are the test game learnt from youtube, the first prototype of the game and the final result respectively.
##
INSTALLATION (LINUX):
-

Download the repo onto your computer using one of the following methods:

1. Download ZIP:

Click the green "Code" button at the top right of the repository page and select "Download ZIP"
Extract the ZIP file:
```
unzip Moonlight.zip
cd Moonlight/Proto2
```

2. Clone using Git:

Open a terminal and run:
```
git clone https://github.com/BobbyBob-Game/Moonlight.git
cd Moonlight/Proto2
```
RUN THE GAME: ```./build/build.sh```
##

The game is ran using a bash script, which compile every .cpp file and the entire include directory with linkers to the necessary SDL2 library into the ./bin/main

##
Check list:
-
- SDL functions
- Parallax background
- Texture
- Background
- Event keyboard
- Animation
- Collision
- Score
- Sound
- Background music
- Music on, off
- Font
- Tile collision
- Player-everything collisions

--MORE--
-
- SDL_Vertex for rendering triangle
- Mode selection through dialogue
- Dialogue options
- Smooth animation for actions
- Text rendering
- Player
- Enemy
- Menus
- Pause/Resume
- Holding Jump
- Dash mechanics
- Vertical level
- A* search algorithm
- Dialogue
- Multiple tile map made using CSV file and Tiled Map Editor
- Loading CSV files 
- Multiple map load using csv files, and Tiled Map Editor
- Widget for making menu
- File separation
##
