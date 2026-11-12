#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBase64Checks.cmake
#
# PURPOSE:
#   Register allocation-free base64 admission and installed consumer material.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(BUILD_TESTING)
    add_executable(umicom-base64-admission-test "${CMAKE_CURRENT_LIST_DIR}/../tests/test_base64_admission.c")
    target_link_libraries(umicom-base64-admission-test PRIVATE Umicom::base)
    umicom_apply_warnings(umicom-base64-admission-test)
    umicom_apply_sanitizers(umicom-base64-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-base64-admission-test)
    endif()
    foreach(_case IN ITEMS
        empty one two three four five six zero whitespace whitespace-only space-strict single unpadded-two unpadded-three padding-missing padding-leading padding-second padding-middle padding-only padding-excess after-padding after-padding-space one-pad-bits two-pad-bits punctuation url-dash url-underscore embedded-zero non-ascii unknown-flags capacity exact-capacity exact-span null-empty null-input null-output null-count overlap count-alias long)
        add_test(NAME "framework.base64.admission.${_case}"
            COMMAND umicom-base64-admission-test "${_case}")
        set_tests_properties("framework.base64.admission.${_case}" PROPERTIES
            TIMEOUT 10 LABELS "framework;base64;encoding;ownership;regression")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../examples/base64_admission/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/base64_admission/CMakeLists.txt"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/base64_admission"
    COMPONENT FrameworkSDK)
