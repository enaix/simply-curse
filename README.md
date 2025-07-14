## simply curse

Yet another TUI ncurses replacement library, because why not. Designed for hacking together an interactive UI. 

### Features

- Uses C++20 standard
- Allows writing custom widget logic using `funcptr` with lambdas
- Handles widget events, window rendering and layouts for you, but you may override this if you want.
- Burns extra CPU cycles, but I may fix that in the future.
- Eyecandy: customizable palettes and window borders

### Building

Note: the library is in early WIP!

Add the following lines to the `CMakeLists.txt`:

`add_subdirectory(path-to-curse)`

`target_link_libraries(your-target INTERFACE curse)`

