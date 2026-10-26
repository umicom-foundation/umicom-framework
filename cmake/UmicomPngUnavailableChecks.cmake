# Umicom Framework
# File: cmake/UmicomPngUnavailableChecks.cmake
# PURPOSE: Exercise real provider-disabled PNG code even when the host has libpng.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
# Compile the production adapter in a separate fixture with no optional provider.
# Do not link the enabled PNG target: that would test the current host capability
# again and leave the unavailable branch skipped. Existing host tests remain.
add_library(umi-png-unavailable-runtime STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../src/media/png_image.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/media/png_memory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/media/png_encode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/media/png_file.c")
target_link_libraries(umi-png-unavailable-runtime PUBLIC Umicom::media_engine Umicom::platform)
set_target_properties(umi-png-unavailable-runtime PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umi-png-unavailable-runtime PRIVATE /UUMICOM_MEDIA_PNG_ENABLED)
else()
    target_compile_options(umi-png-unavailable-runtime PRIVATE -UUMICOM_MEDIA_PNG_ENABLED)
endif()
umicom_apply_warnings(umi-png-unavailable-runtime)
umicom_apply_sanitizers(umi-png-unavailable-runtime)
foreach(kind image encode)
    add_executable(umi-png-off-${kind}-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/media/test_png_${kind}.c")
    target_link_libraries(umi-png-off-${kind}-test PRIVATE umi-png-unavailable-runtime)
    set_target_properties(umi-png-off-${kind}-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-png-off-${kind}-test)
    umicom_apply_sanitizers(umi-png-off-${kind}-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-png-off-${kind}-test)
    endif()
    set(test framework.media.png.provider-disabled.${kind})
    add_test(NAME ${test} COMMAND umi-png-off-${kind}-test unavailable)
    # No skip code: accidental provider activation is a fixture failure.
    set_tests_properties(${test} PROPERTIES TIMEOUT 30
        LABELS "framework;media;png;provider-disabled;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umi-png-off-${kind}-test)
    endif()
endforeach()
add_executable(umi-png-off-contract-test
    "${CMAKE_CURRENT_LIST_DIR}/../tests/media/test_png_unavailable.c")
target_link_libraries(umi-png-off-contract-test PRIVATE umi-png-unavailable-runtime)
set_target_properties(umi-png-off-contract-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umi-png-off-contract-test)
umicom_apply_sanitizers(umi-png-off-contract-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-png-off-contract-test)
endif()
foreach(case availability decode decode-cancelled decode-null-input decode-null-output
    decode-live-output decode-limit encode encode-cancelled encode-null-input encode-null-output
    encode-null-size encode-live-output write write-cancelled write-existing write-relative write-null-result)
    set(test framework.media.png.provider-disabled.contract.${case})
    add_test(NAME ${test} COMMAND umi-png-off-contract-test ${case})
    set_tests_properties(${test} PROPERTIES TIMEOUT 30
        LABELS "framework;media;png;provider-disabled;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umi-png-off-contract-test)
    endif()
endforeach()
