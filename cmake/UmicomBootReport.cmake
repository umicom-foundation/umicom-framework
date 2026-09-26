#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | Licence: MIT
# Normal user-space report interpretation only. No privileged boot source is
# linked into Framework; that implementation remains in the umicom-os repo.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT TARGET umicom_platform)
    message(FATAL_ERROR "Boot report reader requires the canonical platform target")
endif()
target_sources(umicom_platform PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/platform/boot_report.c")
if(BUILD_TESTING)
    add_executable(umicom-boot-report-tests "${CMAKE_CURRENT_LIST_DIR}/../tests/os_foundation/test_boot_report.c")
    target_link_libraries(umicom-boot-report-tests PRIVATE Umicom::platform)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-boot-report-tests)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-boot-report-tests)
    endif()
    foreach(case IN ITEMS ready starting requested failure order duplicate missing version count source control unknown unchanged truncation states fuzz)
        add_test(NAME "framework.os_foundation.boot_report.${case}" COMMAND umicom-boot-report-tests "${case}")
        set_tests_properties("framework.os_foundation.boot_report.${case}" PROPERTIES
            TIMEOUT 15 LABELS "framework;os-foundation;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-boot-report-tests)
    endif()
endif()
