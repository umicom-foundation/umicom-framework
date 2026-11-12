#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomMediaCommands.cmake
#
# PURPOSE:
#   Bind native distribution commands to existing Framework services.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(NOT TARGET umicom)
    return()
endif()
if(NOT TARGET Umicom::boot_media)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomBootMedia.cmake")
endif()
target_sources(umicom PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_media.c")
target_link_libraries(umicom PRIVATE Umicom::boot_media Umicom::setup_centre)
if(TARGET Umicom::setup_process)
    target_link_libraries(umicom PRIVATE Umicom::setup_process)
    target_compile_definitions(umicom PRIVATE UMICOM_CLI_SETUP_HAS_PROCESS=1)
endif()

# Compare command routing only. These cases never open an image, create an
# installation, enumerate a disk or invoke an external media tool.
if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    foreach(_media_command IN ITEMS boot-media setup)
        if(_media_command STREQUAL "boot-media")
            set(_media_standalone umicom-media)
        else()
            set(_media_standalone umicom-setup)
        endif()
        add_test(NAME framework.command.${_media_command}.equivalence
            COMMAND "${CMAKE_COMMAND}"
                "-DUMICOM=$<TARGET_FILE:umicom>"
                "-DSTANDALONE=$<TARGET_FILE:${_media_standalone}>"
                "-DKIND=${_media_command}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/boot_media/check_command_routes.cmake")
        set_tests_properties(framework.command.${_media_command}.equivalence PROPERTIES
            TIMEOUT 45 LABELS "framework;command;media;no-devices")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.command.${_media_command}.equivalence
                TARGETS umicom ${_media_standalone})
        endif()
    endforeach()
endif()
