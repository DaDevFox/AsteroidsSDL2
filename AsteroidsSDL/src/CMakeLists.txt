cmake_minimum_required(VERSION 3.5)
file(STRINGS config.txt CONFIG)

# set project name from config.txt
list(POP_FRONT CONFIG ProjName)
project(${ProjName})

# set project version
list(POP_FRONT CONFIG ProjVersion)
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)"
       ProjVersion_MATCH ${ProjVersion})
set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})

option(DEVELOPMENT_DEPLOY "Deploy to ./redist instead of to the system" ON)

set(${CMAKE_BUILD_TYPE} "Debug")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 23)

message("Generating with config types: ${CMAKE_CONFIGURATION_TYPES}")


file(GLOB asteroids_SRC
     "*.h"
     "*.cpp"
     "math/*.h"
     "math/*.cpp"
     "entities/env/*.h"
     "entities/env/*.cpp"
     "entities/*.h"
     "entities/*.cpp"
     "window/*.h"
     "window/*.cpp")

file(GLOB asteroids_RES 
    "../res/*.png"
    "../res/*.wav"
    "../res/*.ttf")

#TODO: look at https://www.youtube.com/watch?v=6GkZnII8Jss
# from https://github.com/VileLasagna/WarpDrive/blob/dEffectiveMotor/WarpDrive/CMakeLists.txt
if(NOT EXTERNAL_ROOT)
    set(EXTERNAL_ROOT "${CMAKE_CURRENT_DIR}/redist")
endif(NOT EXTERNAL_ROOT)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(WDSYSTEM_ROOT "/usr/local")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WDSYSTEM_ROOT "c:/Program Files/${PROJECT_NAME}")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
list(APPEND CMAKE_PREFIX_PATH ${WDSYSTEM_ROOT})

if ( (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        OR ( (CMAKE_INSTALL_PREFIX STREQUAL EXTERNAL_ROOT) OR (CMAKE_INSTALL_PREFIX STREQUAL WDSYSTEM_ROOT) ) )

    if(DEVELOPMENT_DEPLOY)
        set(CMAKE_INSTALL_PREFIX "${EXTERNAL_ROOT}" CACHE PATH "Default install path" FORCE)
    else(DEVELOPMENT_DEPLOY)
        set(CMAKE_INSTALL_PREFIX "${WDSYSTEM_ROOT}" CACHE PATH "Default install path" FORCE)
    endif(DEVELOPMENT_DEPLOY)

endif()
set(LIB_INSTALL_DIR lib)
set(INCLUDE_INSTALL_DIR include)


# CPack generic config
if(WIN32)
    if("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "x64")
        set(ProjSystem "win64")
    else()
        set(ProjSystem "win32")
    endif()
elseif(${CMAKE_SYSTEM_NAME} matches "Darwin")
    set(ProjSystem "macOS")
elseif(${CMAKE_SYSTEM_NAME} matches "Linux")
    set(ProjSystem "linux")
endif(WIN32)

set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_SOURCE_IGNORE_FILES .git/ .github/ .vs/ .vscode/ .mypy_cache/ _CPack_Packages/ ${CMAKE_BINARY_DIR}/ ${PROJECT_BINARY_DIR}/)

set(CPACK_PACKAGE_NAME ${ProjName})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Stealth-simulation game about avoiding enemy ships")

set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Packaging")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})

set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${ProjName})
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${ProjName}-${ProjVersion}")
set(CPACK_PACKAGE_FILE_NAME "${ProjName}-${ProjVersion}-${ProjSystem}")

set(CPACK_MONOLITHIC_INSTALL TRUE)

#set(CPACK_RESOURCE_FILE_WELCOME
#  ${CMAKE_CURRENT_LIST_DIR}/Welcome.txt)
#set(CPACK_RESOURCE_FILE_README
#  ${CMAKE_CURRENT_LIST_DIR}/Readme.txt)
#set(CPACK_RESOURCE_FILE_LICENSE
#  ${CMAKE_CURRENT_LIST_DIR}/License.txt)
#set(CPACK_PACKAGE_DESCRIPTION_FILE
#  ${CMAKE_CURRENT_LIST_DIR}/Description.txt)



if(APPLE)
#    find_library(SDL2 SDL2)
#    find_library(SDL2IMAGE SDL2_image)
#    find_library(SDL2TTF sdl2_ttf)
    
    set(CPACK_GENERATOR "DragNDrop;productbuild")

#    SET(CMAKE_SKIP_BUILD_RPATH FALSE)
#    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
#    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/libs")
#    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/libs" isSystemDir)
#    list(APPEND CMAKE_INSTALL_RPATH
#      "@executable_path/../../Frameworks")

#    set(CMAKE_INSTALL_PREFIX "Library/Frameworks/SDL2.framework")

    set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Packaging")

    find_package(SDL2 REQUIRED)
    find_package(SDL2IMAGE REQUIRED)
    find_package(SDL2TTF REQUIRED)

    add_executable(${ProjName} ${asteroids_SRC})
    
    include_directories(${SDL2_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR})
    target_link_libraries(${ProjName} ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARY} ${SDL2IMAGE_LIBRARY})
    
    list(POP_FRONT ${SDL2_LIBRARIES} SDL2_DYLIB_T)
    list(POP_FRONT ${SDL2IMAGE_LIBRARY} SDL2_IMAGE_DYLIB_T)
    list(POP_FRONT ${SDL2TTF_LIBRARY} SDL2_TTF_DYLIB_T)

    # unsure if libraries themselves are also needed
    install(FILES ${SDL2_DYLIB_T} DESTINATION bin)
    install(FILES ${SDL2_IMAGE_DYLIB_T} DESTINATION bin)
    install(FILES ${SDL2_TTF_DYLIB_T} DESTINATION bin)
    install(FILES ${asteroids_RES} DESTINATION bin)
    
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

    # CPack config

    # Cpack invocation
    include(CPack)

#    install(DIRECTORY ${SDL2_INCLUDE_DIR} 
#        DESTINATION include)
#    install(DIRECTORY ${SDL2TTF_INCLUDE_DIR} 
#        DESTINATION include)
#    install(DIRECTORY ${SDL2Image_INCLUDE_DIR}
#        DESTINATION include)

#    install(DIRECTORY ${SDL2_LIBRARY} DESTINATION lib)

#    install(FILES SDL.h DESTINATION include)
#    install(FILES SDL_image.h DESTINATION include)
#    install(FILES SDL_ttf.h DESTINATION include)

#    target_link_libraries(${PROJECT_NAME} "-framework SDL2")
#    target_link_libraries(${PROJECT_NAME} "-framework SDL2_image")
#    target_link_libraries(${PROJECT_NAME} "-framework SDL2_ttf")
#    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")
else(APPLE)
    # https://github.com/search?q=path%3A%2F%28%5E%7C%5C%2F%29CMakeLists%5C.txt%24%2F+NSIS&type=code :pray:
    add_executable(${PROJECT_NAME} ${asteroids_SRC})
    target_sources(${PROJECT_NAME} PUBLIC
      ${SHARED_STATIC_ASSETS}
      installation/win/icon.ico
      installation/win/app.rc 
      installation/win/App.manifest)
    
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_OUTPUT_FILE_PREFIX packages)


    # TEMP; FOR LOCAL DEV ONLY
    set(CMAKE_PREFIX_PATH CMAKE_PREFIX_PATH "D:/SDL")

    find_package(SDL2 REQUIRED)
    find_package(SDL2IMAGE REQUIRED)
    find_package(SDL2TTF REQUIRED)
    
    message("SDL2: ${SDL2_LIBRARIES}")
    message("SDL2 dll: ${SDL2_DLL}")
    message("SDL2_image: ${SDL2IMAGE_LIBRARY}")
    message("SDL2_image dll: ${SDL2_IMAGE_DLL}")
    message("SDL2_ttf: ${SDL2TTF_LIBRARY}")
    message("SDL2_ttf dll: ${SDL2_TTF_DLL}")

    # link/set includes for executable
    include_directories(${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARY} ${SDL2TTF_LIBRARY})

    # cp files into installler (installer will copy back out on install)
    install(FILES ${SDL2_DLL} DESTINATION bin)
    install(FILES ${SDL2_IMAGE_DLL} DESTINATION bin)
    install(FILES ${SDL2_TTF_DLL} DESTINATION bin)
    install(FILES ${asteroids_RES} DESTINATION bin)
    
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
    

    # Windows config properties
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${PROJECT_NAME}")
    
    # NSIS config properties
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL YES)

    # NSIS assets

    # Icons
    set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}\\\\installation\\\\win\\\\nsis_icon.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_SOURCE_DIR}\\\\installation\\\\win\\\\icon.ico")

    # Header, Installer/Uninstaller Welcome
    # set(CPACK_NSIS_MUI_HEADERIMAGE ${CMAKE_CURRENT_LIST_DIR}/nsis\\\\nsis_header.bmp)
 #    set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP
 #      ${CMAKE_CURRENT_LIST_DIR}/nsis\\\\nsis_install_welcome.bmp)
 #    set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP
 #     ${CMAKE_CURRENT_LIST_DIR}/nsis\\\\nsis_uninstall_welcome.bmp)

    
    set(CPACK_NSIS_MANIFEST_DPI_AWARE ON)

    set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin")
    
    #set(CPACK_CREATE_DESKTOP_LINKS ON) 
    # REPLACED BY BELOW (above misses SetOutPath)
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "SetOutPath \"$INSTDIR\\\\bin\" \n CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROJECT_NAME}.lnk' '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe' \n CreateShortCut '$DESKTOP\\\\${PROJECT_NAME}.lnk' '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe' ") 
    set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '$SMPROGRAMS\\\\$START_MENU\\\\${PROJECT_NAME}.lnk'" "Delete '$DESKTOP\\\\${PROJECT_NAME}.lnk'")

    
    # result of https://gitlab.kitware.com/cmake/cmake/-/issues/15982 -- very frustrating bug :(
    set(CPACK_NSIS_MODIFY_PATH "ON") 

    set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}")
    set(CPACK_NSIS_WELCOME_TITLE_3LINES "ON")
    
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${PROJECT_NAME}")

    #set(CPACK_NSIS_CREATE_ICONS_EXTRA 
        #"CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROJECT_NAME}.lnk' '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe'"
    #)
    #set(CPACK_NSIS_DELETE_ICONS_EXTRA
        #"Delete '$SMPROGRAMS\\\\$START_MENU\\\\${PROJECT_NAME}.lnk'"
    #)



    # Cpack invocation
    include(CPack)
endif()
