#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeKernelAbiReport.cmake
#
# PURPOSE:
#   Compose the native Kernel ABI report reader in the existing hosted platform.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
# The reader shares existing platform and ASCII conversion dependencies. It
# adds no link from the freestanding Kernel into the hosted Framework.
target_sources(umicom_platform PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/platform/native_kernel_abi_report.c")
# Install the documented host workflow beside the shared artwork resources.
include(GNUInstallDirs)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/platform/native-kernel-abi-report.md"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/platform" COMPONENT Learning)
if(BUILD_TESTING)
    add_executable(umicom-native-kernel-abi-report-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/os_foundation/test_native_kernel_abi_report.c")
    target_link_libraries(umicom-native-kernel-abi-report-tests PRIVATE Umicom::platform)
    foreach(helper IN ITEMS umicom_apply_warnings umicom_apply_sanitizers umicom_register_validation_target)
        if(COMMAND ${helper})
            cmake_language(CALL ${helper} umicom-native-kernel-abi-report-tests)
        endif()
    endforeach()
    foreach(test IN ITEMS lf crlf future-abi future-schema schema-zero missing-header missing-footer missing-newline
        command-echo trailing-prompt major-zero version-overflow version-dots words byte-order page-power
        call-budget overflow leading-zero embedded-nul duplicate-row unknown-row bound file-valid file-incomplete file-missing)
        add_test(NAME "framework.os_foundation.native_kernel_abi_report.${test}"
            COMMAND umicom-native-kernel-abi-report-tests "${test}")
        set_tests_properties("framework.os_foundation.native_kernel_abi_report.${test}" PROPERTIES
            TIMEOUT 15 LABELS "framework;os-foundation;abi;regression")
    endforeach()
endif()
