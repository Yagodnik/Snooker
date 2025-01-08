cmake_minimum_required(VERSION 3.29)
project(Snooker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)


set(LIBRARIES_DIRECTORY ${CMAKE_SOURCE_DIR}/libraries)

# SFML
set(SFML_INCLUDE ${LIBRARIES_DIRECTORY}/SFML)
set(SFML_LIBS
    ${LIBRARIES_DIRECTORY}/SFML/lib/libsfml-window-s.a
    ${LIBRARIES_DIRECTORY}/SFML/lib/libsfml-graphics-s.a
    ${LIBRARIES_DIRECTORY}/SFML/lib/libsfml-system-s.a
    ${LIBRARIES_DIRECTORY}/SFML/lib/libsfml-audio-s.a
    ${LIBRARIES_DIRECTORY}/SFML/lib/libFLAC.a
    ${LIBRARIES_DIRECTORY}/SFML/lib/libogg.a
    ${LIBRARIES_DIRECTORY}/SFML/lib/libvorbis.a
    ${LIBRARIES_DIRECTORY}/SFML/lib/libvorbisenc.a
    ${LIBRARIES_DIRECTORY}/SFML/lib/libvorbisfile.a
    ${LIBRARIES_DIRECTORY}/SFML/lib/libfreetype.a
)
set(MACOS_FRAMEWORKS
    "-framework CoreFoundation"
    "-framework Cocoa"
    "-framework Carbon"
    "-framework IOKit"
    "-framework CoreGraphics"
    "-framework OpenGL"
)

# Box2D
set(BOX2D_INCLUDE ${LIBRARIES_DIRECTORY}/Box2D/)
set(BOX2D_LIBS
    ${LIBRARIES_DIRECTORY}/Box2D/lib/libbox2d.a
)

# Nlohmann
set(JSON_INCLUDE ${LIBRARIES_DIRECTORY}/nlohmann/)

# Assets
set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/assets")
set(CONFIGS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/configs")
set(CONFIGS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/configs")

add_link_options(-ObjC) # https://github.com/SFML/SFML/issues/2920
include_directories(Snooker PRIVATE ${SFML_INCLUDE} ${BOX2D_INCLUDE} ${JSON_INCLUDE})

add_executable(Snooker src/main.cpp
        src/gameobjects/PoolTable.cpp
        src/utils/PhysicsFactory.cpp
        src/gameobjects/Ball.cpp
        src/utils/ResourceManager.cpp
        src/gameobjects/ForceBar.cpp
        src/gameobjects/GameObject.cpp
        src/gameobjects/BallsBar.cpp
        src/Game.cpp
# Debug tools
        dummy/b2SFMLDraw.h
        src/ContactListener.cpp
        src/RayCastCallback.cpp
)


target_link_libraries(Snooker PRIVATE ${SFML_LIBS} ${BOX2D_LIBS})

if (APPLE)
  target_link_libraries(Snooker PRIVATE ${MACOS_FRAMEWORKS})
endif (APPLE)

add_custom_command(TARGET Snooker POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SOURCE_DIR} ${ASSETS_OUTPUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CONFIGS_SOURCE_DIR} ${CONFIGS_OUTPUT_DIR}
)
