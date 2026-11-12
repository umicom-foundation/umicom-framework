#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/ui_cpp_consumer/CMakeLists.txt
#
# PURPOSE:
#   Qualify installed C UI services from C++ without compiling Framework implementations in the application.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.25)
project(UmicomUiCppConsumer LANGUAGES C CXX)
option(UMICOM_UI_CPP_WITH_GTK "Also qualify installed GTK4 adapter headers and symbols" OFF)
set(components ui ui_components)
if(UMICOM_UI_CPP_WITH_GTK)
    list(APPEND components ui_gtk4)
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS ${components})
include(CTest)
if(BUILD_TESTING)
    # Qualification source is shared with the in-tree matrix, while include paths
    # and link dependencies come exclusively from the selected installed SDK.
    include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/UmicomUiCppConsumer.cmake")
    umicom_add_ui_cpp_checks(umicom-ui-sdk-cpp "" "${UMICOM_UI_CPP_WITH_GTK}")
    if(WIN32)
        enable_language(RC)
    endif()
    foreach(workflow IN ITEMS components enterprise workspace)
        umicom_apply_application_branding(TARGET "umicom-ui-sdk-cpp-${workflow}"
            PRODUCT_NAME "Umicom UI C++ SDK Consumer"
            INTERNAL_NAME "umicom-ui-sdk-cpp-${workflow}" NO_RUNTIME_DEPLOYMENT)
    endforeach()
endif()
