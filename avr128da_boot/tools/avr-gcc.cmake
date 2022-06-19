set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR AVR)

if (WIN32)
  if(DEFINED ENV{AVR_TOOLCHAIN_DIR})
    set(AVR_TOOLCHAIN_DIR $ENV{AVR_TOOLCHAIN_DIR}/)
  else()
    message(FATAL_ERROR "AVR_TOOLCHAIN_DIR Not Defined")
  endif()
  set(TOOLCHAIN_PREFIX ${AVR_TOOLCHAIN_DIR}/bin/avr-)
else()
set(AVR_TOOLCHAIN_DIR $ENV{HOME}/hdd/tools/avr)
set(BINUTILS_PATH ${AVR_TOOLCHAIN_DIR})
set(TOOLCHAIN_PREFIX ${AVR_TOOLCHAIN_DIR}/bin/avr-)
endif()
message("AVR_TOOLCHAIN_DIR $AVR_TOOLCHAIN_DIR")


find_program(CMAKE_MAKE_PROGRAM
  NAMES make
        make.exe
  DOC "Find a suitable make program for building under Windows/MinGW"
  HINTS c:/MinGW-32/bin ) 



set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if (WIN32)
set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc.exe" CACHE FILEPATH "C Compiler path")
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++.exe" CACHE FILEPATH "C++ Compiler path")
else()
set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc" CACHE FILEPATH "C Compiler path")
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++" CACHE FILEPATH "C++ Compiler path")
endif()

set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size tool")

set(CMAKE_C_STANDARD    11)
set(CMAKE_CXX_STANDARD  11)

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)