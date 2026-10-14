#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomHealthcareCore.cmake
# PURPOSE:
#   Own the portable C23 healthcare domain starter and its independent tests.
#   A consuming application may include this module after Framework has been
#   configured; it must not duplicate the underlying patient registry logic.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# Copyright (c) 2026 Umicom Foundation
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)

if(NOT TARGET Umicom::base)
    message(FATAL_ERROR
        "Umicom Healthcare Core requires the canonical Umicom::base target")
endif()

set(_umicom_healthcare_framework_root "${CMAKE_CURRENT_LIST_DIR}/..")

add_library(umicom_healthcare_core STATIC
    "${_umicom_healthcare_framework_root}/src/healthcare/synthetic_registry.c"
    "${_umicom_healthcare_framework_root}/src/healthcare/synthetic_encounter.c"
    "${_umicom_healthcare_framework_root}/src/healthcare/synthetic_routing.c")
add_library(Umicom::healthcare_core ALIAS umicom_healthcare_core)
set_target_properties(umicom_healthcare_core PROPERTIES
    EXPORT_NAME healthcare_core
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_healthcare_core PUBLIC
    $<BUILD_INTERFACE:${_umicom_healthcare_framework_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_healthcare_core PUBLIC Umicom::base)

if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_healthcare_core)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_healthcare_core)
endif()

# Export a real Framework component; the product module does not compile or
# own this source. The registry intentionally handles synthetic IDs only.
install(TARGETS umicom_healthcare_core
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT Framework)
install(FILES
    "${_umicom_healthcare_framework_root}/include/umicom/healthcare/synthetic_registry.h"
    "${_umicom_healthcare_framework_root}/include/umicom/healthcare/synthetic_encounter.h"
    "${_umicom_healthcare_framework_root}/include/umicom/healthcare/synthetic_routing.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/healthcare"
    COMPONENT Framework)

if(BUILD_TESTING)
    add_executable(umicom-healthcare-core-test
        "${_umicom_healthcare_framework_root}/tests/healthcare/test_synthetic_registry.c")
    target_link_libraries(umicom-healthcare-core-test PRIVATE
        Umicom::healthcare_core)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-healthcare-core-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-healthcare-core-test)
    endif()
    add_test(NAME framework.healthcare.synthetic_registry
        COMMAND umicom-healthcare-core-test)
    set_tests_properties(framework.healthcare.synthetic_registry PROPERTIES
        TIMEOUT 30 LABELS "framework;healthcare;synthetic;regression")

    # Intake and manually selected worklist routing have separate test targets.
    # They remain independent of any product's frontend or clinical decisions.
    add_executable(umicom-healthcare-encounter-test
        "${_umicom_healthcare_framework_root}/tests/healthcare/test_synthetic_encounter.c")
    target_link_libraries(umicom-healthcare-encounter-test PRIVATE
        Umicom::healthcare_core)
    add_executable(umicom-healthcare-routing-test
        "${_umicom_healthcare_framework_root}/tests/healthcare/test_synthetic_routing.c")
    target_link_libraries(umicom-healthcare-routing-test PRIVATE
        Umicom::healthcare_core)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-healthcare-encounter-test)
        umicom_apply_warnings(umicom-healthcare-routing-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-healthcare-encounter-test)
        umicom_apply_sanitizers(umicom-healthcare-routing-test)
    endif()
    add_test(NAME framework.healthcare.synthetic_encounter
        COMMAND umicom-healthcare-encounter-test)
    add_test(NAME framework.healthcare.synthetic_routing
        COMMAND umicom-healthcare-routing-test)
    set_tests_properties(
        framework.healthcare.synthetic_encounter
        framework.healthcare.synthetic_routing PROPERTIES
        TIMEOUT 30 LABELS "framework;healthcare;synthetic;regression")
endif()

unset(_umicom_healthcare_framework_root)
