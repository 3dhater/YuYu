# YuYu
Game engine with low overhead.

# Features

* Can compile with Visual Studio 2012
* Loading PNG, DDS, BMP
* 2d sprite with animations
* OpenGL 3.3, Direct3D 11

# Information

Code without polymorphism and inheritance. It may be difficult for inexperienced users, but it is needed to increase speed and reduce overhead.

Inspired by Quake 2 and Half-Life 2.

1.  Compile yuyu.dll. It contains general features such as working with archives, default GUI (not imgui), etc. Every memory allocations must be done through yuyu.dll. All objects with `yy` prefix must be created using yyCreate function, and destroyed by yyDestroy.

2. Compile video drivers. Every video driver must implement functions and set callback in yyVideoDriverAPI object. Drawing must be in render target texture. Function `BeginDraw` must set render target this RTT\FBO. `EndDraw` must set render target - default screen, and draw rectangle with our RTT\FBO. `SwapBuffers` it's SwapBuffers\Present.
See opengl plugin for example.

3. game.exe - application with platform specific code. Add .cpp files from common/os_name into your project. 

# Games

Soon

