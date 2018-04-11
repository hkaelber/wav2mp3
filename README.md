# wav2mp3

A small POC command line tool leveraging the libmp3lame library for converting
wave-files to the mp3 format.

## BUILD

The code can be built both on Linux and Windows (MinGW 32).

### COMMON DEPENDENCIES

- GNU C++ compiler (C++11)
- libstdc++
- pthreads
- libmp3lame (>= 3.100)
- CMake (>= 3.1)

### COMMON BUILD OPTIONS

wav2mp3 requires a new libmp3lame (>=3.100). In order to use this library from another location, use the corresponding cmake variables. Example:

```
cmake .. -DCMAKE_LIBRARY_PATH=/path/to/my/lame/lib -DCMAKE_INCLUDE_PATH=/path/to/my/lame/include
```

For additional debugging output and non-optimized compilation build in Debug
mode:

```
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### Linux

In its simplest form build the project as follows:

```
mkdir build
cd build
cmake ..
make -j4
sudo make install
```

### Windows/MinGW

On Windows platforms you need to install the MinGW(32) development environment
including all libaries mentioned above.

Build the code with something like the following:

```
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

## RUN

Run the command line tool passing the directory containing wave-files that
shall be converted to mp3, e.g. like this:

```
./wav2mp3 /path/to/my/wav/files
```

It will convert all files with the extension .wav to .mp3 files in the passed
directory.

## POSSIBLE IMPROVEMENTS / TODOS

- Support wave-formats with != 16 bits per sample
- Improve performance by using different flavors of lame_encode_buffer_XXX().
- Unit tests?!
