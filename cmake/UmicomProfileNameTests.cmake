#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomProfileNameTests.cmake
#
# PURPOSE:
#   Register pure profile-name regression cases without account backends.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
add_executable(umicom-profile-name-tests "${CMAKE_CURRENT_LIST_DIR}/../tests/profile_name/test_profile_name.c")
target_link_libraries(umicom-profile-name-tests PRIVATE Umicom::profile_name)
set_target_properties(umicom-profile-name-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-profile-name-tests)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-profile-name-tests)
endif()
# A cross-compiled executable needs an explicit target runner. Do not silently
# register it as a host command; target-device/browser evidence is separate.
if(NOT CMAKE_CROSSCOMPILING OR CMAKE_CROSSCOMPILING_EMULATOR)
    foreach(case alphabet bounds overlap strings nulls)
        add_test(NAME "framework.profile_name.${case}" COMMAND umicom-profile-name-tests "${case}")
        set_tests_properties("framework.profile_name.${case}" PROPERTIES
            TIMEOUT 15 LABELS "framework;security;portable;regression")
    endforeach()
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-profile-name-tests)
endif()
