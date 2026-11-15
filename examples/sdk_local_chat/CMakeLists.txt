#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/sdk_local_chat/CMakeLists.txt
#
# PURPOSE:
# Build an independent branded local streaming client against an installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomSdkLocalChat VERSION 1.0.0 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(UMICOM_SDK_ROOT "" CACHE PATH "Installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "UMICOM_SDK_ROOT must name an absolute existing installed SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS ai_workspace web platform
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::ai_workspace Umicom::web Umicom::platform)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-local-chat main.c)
target_link_libraries(umicom-sdk-local-chat PRIVATE Umicom::ai_workspace Umicom::platform)
umicom_apply_application_branding(TARGET umicom-sdk-local-chat
    PRODUCT_NAME "Umicom Local Chat SDK Example" INTERNAL_NAME "umicom-sdk-local-chat"
    APPLICATION_ID "org.umicom.sdk.local.chat" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
