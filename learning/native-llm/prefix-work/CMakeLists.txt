#-----------------------------------------------------------------------------
# Umicom Framework — Native LLM Prefix-Reuse Laboratory
# File: learning/native-llm/prefix-work/CMakeLists.txt
# PURPOSE:
#   Opt-in isolated C23 project on top of the SAME Framework-owned training
#   and KV-cache modules. Existing native-LLM targets stay unchanged.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeLlmPrefixReuse LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
get_filename_component(_umicom_framework_root
    "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
include("${_umicom_framework_root}/cmake/UmicomNativeLlmTraining.cmake")
include("${_umicom_framework_root}/cmake/UmicomNativeLlmPrefixReuse.cmake")
