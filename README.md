# EasyPad

A minimalistic notepad application written in C++.

## Requirements
You will need a compiler and Qt installed.
```
brew install qt
```

## Build for MacOs

```
rm -rf build
cmake -S . -B build -DCMAKE_PREFIX_PATH=$(brew --prefix qt)
cmake --build build. 
```
