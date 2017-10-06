#Project name
set(PRJ_NAME F107VCT6)
#Enter MCU model here
set(MCU_NAME STM32F107VC)
#Enter MCU value line here
set(MCU_VALUE_LINE STM32F10x_CL)
#Path to local toolchain directory
set(TOOLCHAIN_DIR "/usr/local/gcc-arm-none-eabi-5_4-2016q3/bin") #Maybe no need it within Qt Creator
set(LINKER_SCRIPT_NAME arm-gcc-link)
#MCU architecture
set(ARCH cortex-m3)
#FPU operations hard or soft
set(FLOAT_ABI soft)
#FPU type
set(FPU vfp)#fpv4-sp-d16)
#End of setup

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

find_program(ARM_CC arm-none-eabi-gcc ${TOOLCHAIN_DIR})
find_program(ARM_AS arm-none-eabi-as ${TOOLCHAIN_DIR})
find_program(ARM_CXX arm-none-eabi-g++ ${TOOLCHAIN_DIR})
find_program(ARM_OBJCOPY arm-none-eabi-objcopy ${TOOLCHAIN_DIR})
find_program(ARM_SIZE_TOOL arm-none-eabi-size ${TOOLCHAIN_DIR})

#Specify cross compiler
include(CMakeForceCompiler)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER  ${ARM_CC})
set(CMAKE_CXX_COMPILER ${ARM_CXX})
#Set linker script
set(CMAKE_LINKER ${ARM_CC})
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/ld/${LINKER_SCRIPT_NAME}.ld)
#Common compiler flags
set(COMMON_FLAGS "-mcpu=${ARCH} -mthumb  -mfloat-abi=${FLOAT_ABI} -mfpu=${FPU} -Wall -g -O0 -ffunction-sections") #--specs=nano.specs")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=c11")

set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${ARCH} -nostartfiles  -O0 -gc-sections -mfloat-abi=${FLOAT_ABI} -mfpu=${FPU} -mthumb -Wall -lm -lgcc -lc -T ${LINKER_SCRIPT}")


#Project name here
project(${PRJ_NAME} ASM C)
cmake_minimum_required(VERSION 2.8)
add_definitions(-D${MCU_NAME})
add_definitions(-DUSE_STDPERIPH_DRIVER -DASSEMBLY -DFPU_USED -DFPU_PRESENT -DARM_MATH_CM3)

#include paths
include_directories("user_files/main")
include_directories("user_files/init")
include_directories("user_files/usart")
#include_directories("user_files/init/usb")
#include_directories("user_files/dbg_console")
include_directories("cmsis")
#include_directories("premain")
include_directories("spl/inc")
include_directories("spl/src")
include_directories("startup")


include_directories("cyclone_tcp_ssl_crypto")
include_directories("cyclone_tcp_ssl_crypto/common")
include_directories("cyclone_tcp_ssl_crypto/cyclone_crypto")
include_directories("cyclone_tcp_ssl_crypto/cyclone_ssl")
include_directories("cyclone_tcp_ssl_crypto/cyclone_tcp")
include_directories("cyclone_tcp_ssl_crypto/config")


#Add all files in sub dirs
file(GLOB_RECURSE TCP_F
                 "cyclone_tcp_ssl_crypto/cyclone_tcp/core/*"
                 "cyclone_tcp_ssl_crypto/cyclone_tcp/ipv4/*"
                 "cyclone_tcp_ssl_crypto/cyclone_tcp/dhcp/*"
                 "cyclone_tcp_ssl_crypto/cyclone_tcp/dns/*"
                 "cyclone_tcp_ssl_crypto/cyclone_tcp/netbios/*"
                 "cyclone_tcp_ssl_crypto/config/*"
)


#Add a separate files in sub dirs
list(APPEND  TCP_L "cyclone_tcp_ssl_crypto/common/date_time"
                 "cyclone_tcp_ssl_crypto/common/debug"
                 "cyclone_tcp_ssl_crypto/common/endian"
                 "cyclone_tcp_ssl_crypto/common/os_port_none"
                 "cyclone_tcp_ssl_crypto/common/str"
	         "cyclone_tcp_ssl_crypto/common/compiler_port.h"
	         "cyclone_tcp_ssl_crypto/common/error"
	         "cyclone_tcp_ssl_crypto/common/os_port.h"

                 "cyclone_tcp_ssl_crypto/cyclone_tcp/drivers/ksz8081"
                 "cyclone_tcp_ssl_crypto/cyclone_tcp/drivers/stm32f107_eth"

    )

#include files
file(GLOB_RECURSE USER_FILES "user_files/*")
file(GLOB_RECURSE CMSIS "cmsis/*")
#file(GLOB_RECURSE PREMAIN "premain/*")
file(GLOB_RECURSE SPL_H "spl/inc/*.h")
file(GLOB_RECURSE SPL_S "spl/src/*.c")
#file(GLOB_RECURSE USB_LIB "usb/*")
file(GLOB_RECURSE START_UP_FILE "startup/*")

add_executable(${PROJECT_NAME}.elf

    #User files
    ${USER_FILES}
    #Libraries
    ${SPL_H} ${SPL_S}
    ${CMSIS} 	${TCP_L} ${TCP_F} 			#${PREMAIN} ${SYSCALLS}
    #Startup and linker script files
    ${START_UP_FILE} ${LINKER_SCRIPT}
)











































