#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCompilerPlanChecks.cmake
#
# PURPOSE:
#   Register owner-run compiler preview admission and JSON output checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING AND TARGET umicom)
    add_executable(umicom-cli-compiler-plan-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/test_cli_compiler_plan.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_compiler.c")
    target_include_directories(umicom-cli-compiler-plan-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src")
    target_link_libraries(umicom-cli-compiler-plan-test PRIVATE Umicom::Framework)
    umicom_apply_warnings(umicom-cli-compiler-plan-test)
    umicom_apply_sanitizers(umicom-cli-compiler-plan-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-cli-compiler-plan-test)
    endif()
    add_test(NAME framework.cli.compiler.admission COMMAND umicom-cli-compiler-plan-test)
    set_tests_properties(framework.cli.compiler.admission PROPERTIES
        TIMEOUT 30 LABELS "framework;cli;compiler;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.cli.compiler.admission TARGETS umicom-cli-compiler-plan-test)
    endif()
    foreach(_provider IN ITEMS gcc clang msvc)
        foreach(_action IN ITEMS compile check)
            foreach(_language IN ITEMS c cpp)
                set(_name "framework.cli.compiler.plan.${_provider}.${_action}.${_language}")
                add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
                    "-DUMICOM=$<TARGET_FILE:umicom>" "-DPROVIDER=${_provider}"
                    "-DACTION=${_action}" "-DLANGUAGE=${_language}"
                    -P "${CMAKE_CURRENT_LIST_DIR}/../tests/check_cli_compiler_plan.cmake")
                set_tests_properties("${_name}" PROPERTIES
                    TIMEOUT 15 LABELS "framework;cli;compiler;json;regression")
                if(COMMAND umicom_test_impact_owners)
                    umicom_test_impact_owners("${_name}" TARGETS umicom)
                endif()
            endforeach()
        endforeach()
    endforeach()
    # Separate cases make a missing structured code-generation switch visible
    # without requiring any installed compiler or changing the existing matrix.
    foreach(_provider IN ITEMS gcc clang)
        foreach(_language IN ITEMS c cpp)
            set(_name "framework.cli.compiler.plan.${_provider}.pic.${_language}")
            add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
                "-DUMICOM=$<TARGET_FILE:umicom>" "-DPROVIDER=${_provider}"
                "-DACTION=compile" "-DLANGUAGE=${_language}" "-DPOSITION_INDEPENDENT=ON"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/check_cli_compiler_plan.cmake")
            set_tests_properties("${_name}" PROPERTIES
                TIMEOUT 15 LABELS "framework;cli;compiler;json;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("${_name}" TARGETS umicom)
            endif()
        endforeach()
    endforeach()
endif()
