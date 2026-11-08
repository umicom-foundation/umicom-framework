#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBuildExecution.cmake
#
# PURPOSE:
#   Register workflow execution, output capture and CLI diagnostic contracts.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
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

# Compile the real CLI adapter against a synthetic discovery boundary. This
# verifies that inventory mode cannot enter full environment preparation.
if(BUILD_TESTING)
    add_executable(umicom-doctor-versions-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/test_doctor_versions.c")
    target_link_libraries(umicom-doctor-versions-test PRIVATE Umicom::toolchain)
    set_target_properties(umicom-doctor-versions-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-doctor-versions-test)
    umicom_apply_sanitizers(umicom-doctor-versions-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-doctor-versions-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-doctor-versions-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_check.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/cli.h")
    endif()
    foreach(_case versions missing duplicate mixed null-vector null-value empty-value
            negative-argc help null-context)
        add_test(NAME "framework.toolchain.doctor.${_case}"
            COMMAND umicom-doctor-versions-test "${_case}")
        set_tests_properties("framework.toolchain.doctor.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;toolchain;cli;regression")
        if(_case STREQUAL "versions" OR _case STREQUAL "missing")
            set_tests_properties("framework.toolchain.doctor.${_case}" PROPERTIES
                PASS_REGULAR_EXPRESSION "Compile, link, runtime and C23 probes: NOT RUN")
        endif()
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.toolchain.doctor.${_case}"
                TARGETS umicom-doctor-versions-test)
        endif()
    endforeach()
endif()
