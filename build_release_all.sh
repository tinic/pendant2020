#/bin/sh
mkdir -p build/debugging
cd build/debugging
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=../../arm-gcc-toolchain.cmake -DCMAKE_BUILD_TYPE=Debug ../..
ninja
cd ../..
mkdir -p build/bootloaded
cd build/bootloaded
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=../../arm-gcc-toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DBOOTLOADED=1 ../..
ninja
cd ../..
mkdir -p build/bootloader
cd build/bootloader
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=../../arm-gcc-toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DBOOTLOADER=1 ../..
ninja
