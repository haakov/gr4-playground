# gr4-playground

A starting point for communicating with GNU Radio 4. Look at frontend/script.js for examples

How to run: 

```
docker run -it -p 8080:8080 ghcr.io/haakov/gr4-playground:latest /
```

## Building for Linux

```
cd backend/build
cmake -DCMAKE_BUILD_TYPE=Debug -DSPLIT_BLOCK_INSTANTIATIONS=ON ..
make -j6
```

## Building for WASM

todo

## Building for MacOS

Install dependencies:
```
brew install llvm@20
```

Compile:
```
export CC=/opt/homebrew/opt/llvm@20/bin/clang
export CXX=/opt/homebrew/opt/llvm@20/bin/clang++
export SDKROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
cd backend/build
cmake \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
    -DDISABLE_EXTERNAL_DEPS_WARNINGS=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DUSE_CCACHE=ON\
    -DENABLE_TESTING=ON \
    -DENABLE_COVERAGE=OFF \
    -DCMAKE_OSX_SYSROOT="$SDKROOT" \
    -DCMAKE_EXE_LINKER_FLAGS="-L/opt/homebrew/opt/llvm@20/lib/c++ -Wl,-rpath,/opt/homebrew/opt/llvm@20/lib/c++" \
    -DCMAKE_SHARED_LINKER_FLAGS="-L/opt/homebrew/opt/llvm@20/lib/c++ -Wl,-rpath,/opt/homebrew/opt/llvm@20/lib/c++" -DSPLIT_BLOCK_INSTANTIATIONS=ON ..
make -j6
```

