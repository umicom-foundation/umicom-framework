#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/command_feedback/CMakeLists.txt
# PURPOSE: Build the Notes command-feedback example with the Framework SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomCommandFeedbackExample LANGUAGES C)
if(NOT TARGET Umicom::application)
    find_package(UmicomFramework CONFIG REQUIRED)
endif()
add_executable(umicom-command-feedback-example main.c)
set_target_properties(umicom-command-feedback-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
target_link_libraries(umicom-command-feedback-example PRIVATE Umicom::application)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-command-feedback-example)
    umicom_apply_sanitizers(umicom-command-feedback-example)
endif()
if(BUILD_TESTING)
    add_test(NAME framework.command_feedback.example COMMAND umicom-command-feedback-example)
endif()
install(TARGETS umicom-command-feedback-example RUNTIME DESTINATION bin)
