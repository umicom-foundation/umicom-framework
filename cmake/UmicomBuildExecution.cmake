#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBuildExecution.cmake
# Purpose: Register complete CLI output capture and deterministic workflow checks.
# Author: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
target_sources(umicom_toolchain PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/build_invocation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/build_logging.c")

if(BUILD_TESTING)
    foreach(_case invocation execution logging clean_bounds incremental)
        add_executable("umicom-build-${_case}-test"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/test_build_${_case}.c")
        target_link_libraries("umicom-build-${_case}-test" PRIVATE Umicom::toolchain)
        umicom_apply_warnings("umicom-build-${_case}-test")
        umicom_apply_sanitizers("umicom-build-${_case}-test")
        add_test(NAME "framework.toolchain.build.${_case}" COMMAND "umicom-build-${_case}-test")
        set_tests_properties("framework.toolchain.build.${_case}" PROPERTIES
            LABELS "framework;toolchain;workflow;regression" TIMEOUT 60)
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.toolchain.build.${_case}"
                TARGETS "umicom-build-${_case}-test")
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("umicom-build-${_case}-test")
        endif()
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/development-command.html"
    "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/incremental-builds.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/guides" COMPONENT Framework)
