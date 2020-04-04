# Copyright 2019 Tinic Uro
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
cmake_minimum_required(VERSION 3.10)

# Boot loader
if(BOOTLOADER)
    set(PRJ_NAME pendant2020_bootloader)
    set(CMSISLINKLD STM32F401CCUX_FLASH_BOOTLOADER.ld)
    set(BASE_ADDRESS 0x08000000)
    message(status "Building bootloader binary")
elseif(BOOTLOADED)
    set(PRJ_NAME pendant2020_bootloaded)
    set(CMSISLINKLD STM32F401CCUX_FLASH_BOOTLOADED.ld)
    set(BASE_ADDRESS 0x08008000)
    message(status "Building bootloaded binary")
else(BOOTLOADER)
    set(PRJ_NAME pendant2020_debugging)
    set(CMSISLINKLD STM32F401CCUX_FLASH.ld)
    set(BASE_ADDRESS 0x08000000)
    message(status "Building development binary")
endif(BOOTLOADER)

project(${PRJ_NAME} C CXX ASM)

set(ARM_ARCH cortex-m4)
set(STM32ARCH STM32F4)
set(STM32MCU STM32F401xC)

# CMSIS
set(CMSISSTARTUPS startup_stm32f401ccux.S)
set(CMSISSYSTEMC system_stm32f4xx.c)

include_directories(.)
include_directories(stm32/Inc)
include_directories(stm32/Drivers/CMSIS/Include)
include_directories(stm32/Drivers/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(stm32/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc/)
include_directories(stm32/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/)

set(LINKLDPATH stm32/${CMSISLINKLD})
set(DEVICESTARTUPPATH stm32/${CMSISSTARTUPS})
set(SYSTEMCPATH stm32/Src/${CMSISSYSTEMC})

# HAL
include_directories(stm32/Drivers/STM32F4xx_HAL_Driver/Inc)
file(GLOB_RECURSE HAL_SRC stm32/Drivers/STM32F4xx_HAL_Driver/Src/*.c)
file(GLOB_RECURSE MSC_SRC stm32/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/*.c)
file(GLOB_RECURSE USB_SRC stm32/Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c)
file(GLOB_RECURSE STM32_SRC stm32/Src/*.c)

# LwIP
set(COMMON_FLAGS "-DPENDANT2020 -DSTM32 -DNVIC_BASE_ADDRESS=${BASE_ADDRESS} -D${STM32ARCH} -D${STM32MCU} \
                  -Wall -Wextra -Wno-error=format -Wno-strict-aliasing -mtune=${ARM_ARCH} \
                  -mcpu=${ARM_ARCH} -ffunction-sections -ffast-math -mthumb -mfloat-abi=soft")

if(BOOTLOADER)
    set(COMMON_FLAGS "${COMMON_FLAGS} -DBOOTLOADER=1")
elseif(BOOTLOADED)
    set(COMMON_FLAGS "${COMMON_FLAGS} -DBOOTLOADED=1")
endif(BOOTLOADER)

set(CMAKE_ASM_FLAGS "-mcpu=${ARM_ARCH}")

set(CMAKE_C_FLAGS "${COMMON_FLAGS} -Wno-unused-parameter -std=gnu99")
set(CMAKE_C_FLAGS_RELEASE "-flto -Wl,-flto -Os")
set(CMAKE_C_FLAGS_DEBUG "-Og -g3")

set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -Wall -Wextra \
    -Wno-strict-aliasing -Wdouble-promotion \
    -Wshadow -Wfloat-conversion -Wnull-dereference \
    -Wformat=2 -std=c++17 -fno-rtti -fno-exceptions")
set(CMAKE_CXX_FLAGS_RELEASE "-flto -Wl,-flto -Os")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3")

set(CMAKE_EXE_LINKER_FLAGS "-g3 --specs=nosys.specs \
    --specs=nano.specs -Wl,--warn-unresolved-symbols \
    -Wl,--no-wchar-size-warning -Wl,--start-group -lm \
    -Wl,--end-group -Wl,-gc-sections \
    -T\"${CMAKE_SOURCE_DIR}/${LINKLDPATH}\"")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")

if(BOOTLOADER)
    set(main_SRCS 
        )
else(BOOTLOADER)
    set(main_SRCS
        pendant.cpp
        timeline.cpp
        system_time.cpp
        bq25895.cpp
        )
endif(BOOTLOADER)

set(SOURCE_FILES 
        ${DEVICESTARTUPPATH}
        ${SYSTEMCPATH} 
        ${HAL_SRC} 
        ${STM32_SRC}
        ${MSC_SRC}
        ${USB_SRC}
        ${main_SRCS})

add_executable(${PROJECT_NAME}.elf ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE ${LWIP_INCLUDE_DIRS} ryu)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=\"${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map\"")
set(HEX_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom -R .fuse -R .lock -R .signature $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")

set(PROGRAM_CMD "sudo openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c \"program ${PROJECT_NAME}.bin ${BASE_ADDRESS} verify reset exit\"")
install(CODE "execute_process(COMMAND ${PROGRAM_CMD})")
