#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomProfileNameFfiTests.cmake
#
# PURPOSE:
#   Register portable foreign-language ABI boundary cases without external runtimes.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
add_executable(umicom-profile-name-ffi-test
    "${CMAKE_CURRENT_LIST_DIR}/../tests/profile_name/test_profile_name_ffi.c")
target_link_libraries(umicom-profile-name-ffi-test PRIVATE Umicom::profile_name_ffi)
set_target_properties(umicom-profile-name-ffi-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-profile-name-ffi-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-profile-name-ffi-test)
endif()
if(NOT CMAKE_CROSSCOMPILING OR CMAKE_CROSSCOMPILING_EMULATOR)
    foreach(case version basic exact-span minimum maximum too-short too-long embedded-zero nonascii invalid-leading invalid-character null-input null-output small-capacity zero-capacity excess-capacity overlap partial-overlap readonly-input failure-unchanged padding)
        add_test(NAME "framework.profile_name.ffi.${case}" COMMAND umicom-profile-name-ffi-test "${case}")
        set_tests_properties("framework.profile_name.ffi.${case}" PROPERTIES TIMEOUT 15
            LABELS "framework;security;portable;ffi;regression")
    endforeach()
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-profile-name-ffi-test)
endif()
