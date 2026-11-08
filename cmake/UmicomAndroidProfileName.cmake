#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAndroidProfileName.cmake
#
# PURPOSE:
#   Compose the small Android JNI adapter using the caller-selected NDK toolchain.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(NOT ANDROID)
    message(FATAL_ERROR "The Android profile adapter requires the Android NDK toolchain")
endif()
if(NOT TARGET Umicom::profile_name)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomProfileName.cmake")
endif()
get_target_property(_umi_android_profile_imported Umicom::profile_name IMPORTED)
if(_umi_android_profile_imported)
    message(FATAL_ERROR "The source Android adapter requires a profile-name target built by this NDK project, not an imported archive")
endif()
unset(_umi_android_profile_imported)
if(TARGET Umicom::android_profile_name OR TARGET umicom_android_profile_name)
    message(FATAL_ERROR "Use one Android profile adapter definition")
endif()
add_library(umicom_android_profile_name STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../adapters/android/profile_name.c")
add_library(Umicom::android_profile_name ALIAS umicom_android_profile_name)
set_target_properties(umicom_android_profile_name PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO POSITION_INDEPENDENT_CODE YES)
target_link_libraries(umicom_android_profile_name PUBLIC Umicom::profile_name)
# The NDK supplies jni.h. A host JDK or desktop archive is never selected here.
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom_android_profile_name PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
