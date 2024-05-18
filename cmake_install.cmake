# Install script for directory: D:/Dev/CPPTestProject/AsteroidsSDL/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/redist")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/SDL/lib/x64/SDL2.dll")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/SDL/lib/x64/SDL2_image.dll")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/SDL/lib/x64/SDL2_ttf.dll")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/EncodeSans-Bold.ttf"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/EncodeSans-Light.ttf"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/EncodeSans-Medium.ttf"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/attack_exclamation.png"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/blip_attack.wav"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/blip_warn.wav"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/highlighter_beam.png"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/hit_damage.wav"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/laser_beam.png"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/laser_shoot.wav"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/pip.png"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/thrust.wav"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/thrust_windup.wav"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/triangle.png"
    "D:/Dev/CPPTestProject/AsteroidsSDL/src/../res/warn_exclamation.png"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Dev/CPPTestProject/Debug/AsteroidsSDL.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Dev/CPPTestProject/Release/AsteroidsSDL.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Dev/CPPTestProject/MinSizeRel/AsteroidsSDL.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Dev/CPPTestProject/RelWithDebInfo/AsteroidsSDL.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/Dev/CPPTestProject/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
