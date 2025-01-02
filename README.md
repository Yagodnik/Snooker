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
Assets from https://opengameart.org/content/asset-pack-8-ball-pool