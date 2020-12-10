# YuYu
Game engine with low overhead.

# Features

* Can compile with Visual Studio 2012
* Loading PNG, DDS
* 2d sprite

# Information

Code without polymorphism and inheritance. It may be difficult for inexperienced users, but it is needed to increase speed and reduce overhead.


yuyu.dll - 100% platform independent code

game.exe - application with platform specific code. Add .cpp files from game/os_name into your project

name.yyvd - YuYu video driver

# TODO

* onWindowSize for video API - need to update GUI projection matrix if window size has been changed