# Snooker
Small 8-bit snooker game written in C++ using SFML and Box2D

# Building
Clone project, in root directory create libraries directory.
In libraries create 3 subdirectories: ```SFML```, ```Box2D```, ```nlohmann```

### SFML
In SFML dir create ```libs``` and copy all libraries for SFML and ```SFML``` for includes

### Box2D 
Similar to SFML, just create ```Box2D``` and ```libs``` directories.

### nlohmann
Copy all required headers for nlohmann json library

### Build
```
mkdir build
cd build
cmake ..
cmake --build .
```

# Credits
Assets from https://opengameart.org/content/asset-pack-8-ball-pool \

Sounds & Music: \
https://www.zapsplat.com/music/medium-audience-applauding-front-mics/
https://www.youtube.com/watch?v=fylkRc2DdWM
https://www.zapsplat.com/music/pool-or-snooker-cue-strike-ball/
https://fonts.google.com/specimen/Jersey+10
https://fonts.google.com/specimen/Jersey+25
https://www.zapsplat.com/music/bold-synth-game-tone-negative-fail/
https://www.zapsplat.com/music/game-sound-bold-and-warm-retro-1970s-synth-tone-positive-end-complete-1/ 