#-----------------------------------------------------------------------------
# Umicom Framework
# File: templates/compiler/c/CMakeLists.txt
#
# PURPOSE:
#   Provide reusable c source for projects created with Umicom tooling.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

# Umicom C23 application template | Sammy Hegab | Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomC23Template LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_executable(umicom-c23 src/main.c)
