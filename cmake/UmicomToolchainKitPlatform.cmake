#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomToolchainKitPlatform.cmake
#
# PURPOSE:
#   Complete the Framework-owned native development-kit control plane with
#   project/application kit selection, cross-target toolchains, SDK/sysroot
#   composition, CPython discovery, provenance, health and machine output.
#
# ARCHITECTURE:
#   Extend the established umicom_toolchain target. Do not create a competing
#   compiler, SDK, environment or application-owned discovery subsystem.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_toolchain)
    message(FATAL_ERROR "UmicomToolchainKitPlatform.cmake requires umicom_toolchain.")
endif()

target_sources(umicom_toolchain PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/cross_toolchain.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/python_runtime.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_discovery.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_selection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_provenance.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_environment.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_cmake.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_json.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add toolchain kit test build helper so parent and application projects apply
    # one consistent rule.
    function(umicom_add_toolchain_kit_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::toolchain)
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;toolchain-kit")
    endfunction()

    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-contract-test
        framework.toolchain_kit.contract
        tests/toolchain_kit/test_kit.c)
    umicom_add_toolchain_kit_test(
        umicom-cross-toolchain-plan-test
        framework.toolchain_kit.cross_toolchain
        tests/toolchain_kit/test_cross_toolchain.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-catalogue-test
        framework.toolchain_kit.catalogue
        tests/toolchain_kit/test_kit_catalogue.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-discovery-test
        framework.toolchain_kit.discovery
        tests/toolchain_kit/test_kit_discovery.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-selection-test
        framework.toolchain_kit.selection
        tests/toolchain_kit/test_kit_selection.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-health-test
        framework.toolchain_kit.health
        tests/toolchain_kit/test_kit_health.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-provenance-test
        framework.toolchain_kit.provenance
        tests/toolchain_kit/test_kit_provenance.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-environment-test
        framework.toolchain_kit.environment
        tests/toolchain_kit/test_kit_environment.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-cmake-test
        framework.toolchain_kit.cmake
        tests/toolchain_kit/test_kit_cmake.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-json-test
        framework.toolchain_kit.json
        tests/toolchain_kit/test_kit_json.c)
    umicom_add_toolchain_kit_test(
        umicom-python-runtime-contract-test
        framework.toolchain_kit.python_runtime
        tests/toolchain_kit/test_python_runtime.c)
endif()

message(STATUS "Umicom developer kits, cross-target toolchains and provenance enabled")

# Admit complete kits before projecting environment text, including every
# inline field even when that field is not itself exported.
if(BUILD_TESTING)
    add_executable(umicom-kit-environment-admission-test
        "${UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT}/tests/toolchain_kit/test_environment_admission.c")
    target_link_libraries(umicom-kit-environment-admission-test PRIVATE Umicom::toolchain)
    set_target_properties(umicom-kit-environment-admission-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-kit-environment-admission-test)
    umicom_apply_sanitizers(umicom-kit-environment-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-kit-environment-admission-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-kit-environment-admission-test FILES
            "${UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT}/src/toolchain/kit/kit_environment.c"
            "${UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT}/src/toolchain/kit/kit.c")
    endif()
    set(_kit_environment_cases no-profile profile profile-failure capacity allocation late-invalid
        null-kit null-output small-record version missing-id ready-without-compiler)
    foreach(_field id display_name profile_id sdk_id host_triple target_triple sysroot cmake_generator
            cmake_toolchain_file environment_profile c_compiler cpp_compiler assembler linker debugger
            cmake ctest ninja git python java javac)
        list(APPEND _kit_environment_cases "field.${_field}")
    endforeach()
    foreach(_case IN LISTS _kit_environment_cases)
        add_test(NAME "framework.toolchain_kit.environment.admission.${_case}"
            COMMAND umicom-kit-environment-admission-test "${_case}")
        set_tests_properties("framework.toolchain_kit.environment.admission.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;toolchain-kit;environment;regression;no-process")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.toolchain_kit.environment.admission.${_case}"
                TARGETS umicom-kit-environment-admission-test)
        endif()
    endforeach()
endif()

# Profile-to-kit construction and declared tool slots remain shared services.
# Synthetic records need no compiler discovery or native process launch.
if(BUILD_TESTING)
    add_executable(umicom-kit-profile-admission-test
        "${UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT}/tests/toolchain_kit/test_profile_admission.c")
    target_link_libraries(umicom-kit-profile-admission-test PRIVATE Umicom::toolchain)
    set_target_properties(umicom-kit-profile-admission-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-kit-profile-admission-test)
    umicom_apply_sanitizers(umicom-kit-profile-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-kit-profile-admission-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-kit-profile-admission-test FILES
            "${UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT}/src/toolchain/kit/kit.c"
            "${UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT}/src/toolchain/profile.c")
    endif()
    set(_kit_profile_cases valid native fallback-display fallback-target host-alias missing-target
        found-compiler missing-compiler ready-without-compiler declared-count
        profile-size profile-version profile-count null-profile missing-profile-id
        long-id long-host sdk-text allocation)
    foreach(_field profile_id display_name target_triple generator)
        list(APPEND _kit_profile_cases "text.${_field}")
    endforeach()
    foreach(_tool c cpp linker debugger cmake ctest ninja git java javac)
        list(APPEND _kit_profile_cases "tool.${_tool}")
    endforeach()
    foreach(_boundary zero boundary last negative unknown count version null)
        list(APPEND _kit_profile_cases "accessor.${_boundary}")
    endforeach()
    foreach(_case IN LISTS _kit_profile_cases)
        add_test(NAME "framework.toolchain_kit.profile.admission.${_case}"
            COMMAND umicom-kit-profile-admission-test "${_case}")
        set_tests_properties("framework.toolchain_kit.profile.admission.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;toolchain-kit;profile;regression;no-process")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.toolchain_kit.profile.admission.${_case}"
                TARGETS umicom-kit-profile-admission-test)
        endif()
    endforeach()
endif()

# Incremental kit composition must admit bounded storage without pretending
# that a partially discovered target is ready. These fixtures run no tools.
if(BUILD_TESTING)
    add_executable(umicom-kit-mutation-admission-test
        "${UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT}/tests/toolchain_kit/test_mutation_admission.c")
    target_link_libraries(umicom-kit-mutation-admission-test PRIVATE Umicom::toolchain)
    set_target_properties(umicom-kit-mutation-admission-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-kit-mutation-admission-test)
    umicom_apply_sanitizers(umicom-kit-mutation-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-kit-mutation-admission-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-kit-mutation-admission-test FILES
            "${UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT}/src/toolchain/kit/kit.c")
    endif()
    set(_kit_mutation_cases)
    foreach(_case ready partial unavailable unknown empty-target null size version allocation exhausted kit-storage last-revision)
        list(APPEND _kit_mutation_cases "cross.${_case}")
    endforeach()
    foreach(_field target_triple compiler_path cpp_compiler_path assembler_path linker_path debugger_path sysroot)
        list(APPEND _kit_mutation_cases "cross.text.${_field}")
    endforeach()
    foreach(_case valid unavailable empty text null size version exhausted)
        list(APPEND _kit_mutation_cases "runtime.${_case}")
    endforeach()
    foreach(_setter sysroot cmake environment)
        foreach(_case valid clear alias limit oversize exhausted null malformed metadata)
            list(APPEND _kit_mutation_cases "${_setter}.${_case}")
        endforeach()
    endforeach()
    foreach(_case cross text metadata)
        list(APPEND _kit_mutation_cases "query.${_case}")
    endforeach()
    foreach(_case IN LISTS _kit_mutation_cases)
        add_test(NAME "framework.toolchain_kit.mutation.admission.${_case}"
            COMMAND umicom-kit-mutation-admission-test "${_case}")
        set_tests_properties("framework.toolchain_kit.mutation.admission.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;toolchain-kit;regression;no-process")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.toolchain_kit.mutation.admission.${_case}"
                TARGETS umicom-kit-mutation-admission-test)
        endif()
    endforeach()
endif()
