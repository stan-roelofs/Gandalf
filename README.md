# Gandalf
![GitHub Workflow Status (with branch)](https://img.shields.io/github/actions/workflow/status/stan-roelofs/Gandalf/cmake.yml?branch=main)
![GitHub](https://img.shields.io/github/license/stan-roelofs/Gandalf)

A Gameboy color emulator with debugging tools. The Gameboy emulation code resides in a seperate repository which can be found here [gandalf-lib](https://github.com/stan-roelofs/gandalf-lib). 


This is a work in progress. If you want to play Gameboy games I recommend finding a better emulator.

## Building
Clone the repository and its submodules

```
git clone --recurse-submodules https://github.com/stan-roelofs/Gandalf.git
```

Generate the build system files using cmake

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

Now build the code
```
cmake --build .
```

If you are building on Windows using MSVC: 
```
cmake --build . --config Release
```

Note 1: OpenGL needs to be installed in order to compile. 

Note 2: Compiling as Release or RelWithDebInfo is recommended. The STL containers used by the emulator can be extremely slow in Debug mode because of iterator debugging. 

## Preview
![gold.png](data/gold.png)

![dkc.png](data/dkc.png)