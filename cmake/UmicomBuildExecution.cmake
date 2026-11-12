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
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/prepared_shell.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/build_invocation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/build_logging.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/version_inventory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/file_observation.c")
# Reuse the protocol layer's JSON escaping; it depends only on base/platform.
# Unicode validation is owned by base, so no document/UI dependency is added.
target_link_libraries(umicom_toolchain PRIVATE Umicom::protocol)

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
            negative-argc help null-context report report-incomplete report-failure report-first
            report-alone report-missing report-relative report-option report-null report-duplicate report-mixed)
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

# Evidence contracts use synthetic discovery records and exclusive temporary
# files. They never discover tools or run a compiler/version command themselves.
if(BUILD_TESTING)
    add_executable(umicom-base-utf8-test "${CMAKE_CURRENT_LIST_DIR}/../tests/base/test_utf8.c")
    target_link_libraries(umicom-base-utf8-test PRIVATE Umicom::base)
    umicom_apply_warnings(umicom-base-utf8-test)
    umicom_apply_sanitizers(umicom-base-utf8-test)
    add_test(NAME framework.base.utf8.contract COMMAND umicom-base-utf8-test)
    set_tests_properties(framework.base.utf8.contract PROPERTIES
        TIMEOUT 30 LABELS "framework;base;unicode;regression")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-base-utf8-test)
    endif()
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.base.utf8.contract TARGETS umicom-base-utf8-test)
    endif()
    set(_inventory_cases incomplete complete escaped unicode maximum-text null-report
        stale-size stale-profile stale-api null-output zero-capacity small-output invalid-status
        tool-count duplicate-kind invalid-kind invalid-state invalid-family invalid-compiler invalid-cpp
        unterminated-path unterminated-version unterminated-profile invalid-utf8 found-count
        required-count missing-count compile-pass link-pass runtime-pass c23-pass c23-capable)
    set(_inventory_save_cases new unicode-path existing invalid-report relative-path write-failure
        close-failure create-failure missing-parent)
    foreach(_suite inventory inventory_save)
        if(_suite STREQUAL "inventory")
            set(_source test_version_inventory.c)
        else()
            set(_source test_version_inventory_save.c)
        endif()
        add_executable("umicom-version-${_suite}-test"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/${_source}")
        target_link_libraries("umicom-version-${_suite}-test" PRIVATE Umicom::toolchain Umicom::protocol)
        set_target_properties("umicom-version-${_suite}-test" PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("umicom-version-${_suite}-test")
        umicom_apply_sanitizers("umicom-version-${_suite}-test")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("umicom-version-${_suite}-test")
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs("umicom-version-${_suite}-test" FILES
                "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/version_inventory.c")
        endif()
        foreach(_case IN LISTS _${_suite}_cases)
            add_test(NAME "framework.toolchain.version_${_suite}.${_case}"
                COMMAND "umicom-version-${_suite}-test" "${_case}")
            set_tests_properties("framework.toolchain.version_${_suite}.${_case}" PROPERTIES
                TIMEOUT 30 LABELS "framework;toolchain;evidence;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("framework.toolchain.version_${_suite}.${_case}"
                    TARGETS "umicom-version-${_suite}-test")
            endif()
        endforeach()
    endforeach()
endif()

# Native file observations share protocol emission and platform I/O. These tests
# are source-authored contracts; qualification executes them in the owner's build.
if(BUILD_TESTING)
    foreach(_suite json_evidence_writer file_observation file_observation_admission observe_files_cli)
        add_executable("umicom-${_suite}-test"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/test_${_suite}.c")
        target_link_libraries("umicom-${_suite}-test" PRIVATE Umicom::toolchain Umicom::protocol)
        set_target_properties("umicom-${_suite}-test" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("umicom-${_suite}-test")
        umicom_apply_sanitizers("umicom-${_suite}-test")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("umicom-${_suite}-test")
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-file_observation_admission-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/file_observation.c")
        umicom_target_qualification_inputs(umicom-observe_files_cli-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_observe_files.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/cli.h")
    endif()
    add_test(NAME framework.protocol.json_evidence_writer COMMAND umicom-json_evidence_writer-test)
    set_tests_properties(framework.protocol.json_evidence_writer PROPERTIES TIMEOUT 30 LABELS "framework;protocol;evidence")
    set(_file_observation_cases capture owned format-bounds save save-existing save-input save-invalid
        missing directory duplicate relative null-path symlink small-limit)
    set(_file_observation_admission_cases count-zero count-excess null-vector null-output occupied-output
        limit-zero limit-excess last-relative last-null last-duplicate last-utf8 budget at-budget maximum-files read-error changed-file)
    set(_observe_files_cli_cases valid default-limit help empty negative null-vector null-value empty-value unknown
        missing-value no-files relative-report duplicate-report duplicate-limit zero negative-limit plus-limit
        decimal-limit overflow excess-limit help-mixed count-excess)
    foreach(_suite file_observation file_observation_admission observe_files_cli)
        foreach(_case IN LISTS _${_suite}_cases)
            add_test(NAME "framework.toolchain.${_suite}.${_case}" COMMAND "umicom-${_suite}-test" "${_case}")
            set_tests_properties("framework.toolchain.${_suite}.${_case}" PROPERTIES TIMEOUT 30
                SKIP_RETURN_CODE 77 LABELS "framework;toolchain;evidence;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("framework.toolchain.${_suite}.${_case}" TARGETS "umicom-${_suite}-test")
            endif()
        endforeach()
    endforeach()
endif()

if(BUILD_TESTING AND UMICOM_BUILD_NATIVE_TOOL)
    foreach(_case stdout report existing missing limit source-destination)
        add_test(NAME "framework.toolchain.observe_files_process.${_case}"
            COMMAND "${CMAKE_COMMAND}" "-DTOOL=$<TARGET_FILE:umicom>" "-DCASE=${_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/check_observe_files_cli.cmake")
        set_tests_properties("framework.toolchain.observe_files_process.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;toolchain;evidence;cli")
    endforeach()
endif()

# Compile the real adapters against synthetic boundaries. These checks never
# discover a toolchain, repair a cache or launch an interactive process.
if(BUILD_TESTING)
    set(_cli_admission_cases
        build.null-vector build.null-entry build.negative build.null-context
        build.help build.help-short build.empty build.unknown build.no-options
        repair.null-vector repair.null-entry repair.negative repair.null-context
        repair.help repair.help-short repair.empty repair.unknown repair.no-options
        repair.dry-run repair.duplicate repair.extra
        shell.null-vector shell.null-entry shell.negative shell.null-context
        shell.help shell.help-short shell.empty shell.unknown shell.no-options
        shell.program shell.extra)
    set(_prepared_shell_cases success default empty-default child-failure
        not-launched cancelled timeout transport null-environment count name value
        duplicate later-malformed last-unterminated)
    foreach(_suite cli_admission prepared_shell)
        if(_suite STREQUAL "cli_admission")
            set(_source test_build_cli_admission.c)
            set(_cases ${_cli_admission_cases})
        else()
            set(_source test_prepared_shell.c)
            set(_cases ${_prepared_shell_cases})
        endif()
        add_executable("umicom-build-${_suite}-test"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/${_source}")
        target_link_libraries("umicom-build-${_suite}-test" PRIVATE Umicom::toolchain)
        set_target_properties("umicom-build-${_suite}-test" PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("umicom-build-${_suite}-test")
        umicom_apply_sanitizers("umicom-build-${_suite}-test")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("umicom-build-${_suite}-test")
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            if(_suite STREQUAL "cli_admission")
                umicom_target_qualification_inputs("umicom-build-${_suite}-test" FILES
                    "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_build.c"
                    "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/cli.h")
            else()
                umicom_target_qualification_inputs("umicom-build-${_suite}-test" FILES
                    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/prepared_shell.c"
                    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/environment.c")
            endif()
        endif()
        foreach(_case IN LISTS _cases)
            add_test(NAME "framework.toolchain.build.${_suite}.${_case}"
                COMMAND "umicom-build-${_suite}-test" "${_case}")
            set_tests_properties("framework.toolchain.build.${_suite}.${_case}" PROPERTIES
                TIMEOUT 30 LABELS "framework;toolchain;cli;regression;no-process")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("framework.toolchain.build.${_suite}.${_case}"
                    TARGETS "umicom-build-${_suite}-test")
            endif()
        endforeach()
    endforeach()
endif()

# Shared environment admission is exercised without writing a file. The fixture
# substitutes only candidate allocation and the final export boundary.
if(BUILD_TESTING)
    add_executable(umicom-environment-admission-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/test_environment_admission.c")
    target_link_libraries(umicom-environment-admission-test PRIVATE Umicom::toolchain)
    set_target_properties(umicom-environment-admission-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-environment-admission-test)
    umicom_apply_sanitizers(umicom-environment-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-environment-admission-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-environment-admission-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/environment.c"
            "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/toolchain/environment.h")
    endif()
    foreach(_case compose alias-base alias-overlay alias-both empty-compose
            invalid-base invalid-overlay allocation compose-capacity alias-capacity
            set-self set-slice add-duplicate invalid-name invalid-value long-value
            count-read count-mutate unterminated-read export export-failure
            export-invalid export-capacity)
        add_test(NAME "framework.toolchain.environment.admission.${_case}"
            COMMAND umicom-environment-admission-test "${_case}")
        set_tests_properties("framework.toolchain.environment.admission.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;toolchain;environment;regression;no-process")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.toolchain.environment.admission.${_case}"
                TARGETS umicom-environment-admission-test)
        endif()
    endforeach()
endif()

# Toolchain profile conversion must distinguish absent optional tools from bad
# validated paths, while retaining the last published environment on failure.
if(BUILD_TESTING)
    add_executable(umicom-toolchain-environment-admission-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/test_toolchain_environment_admission.c")
    target_link_libraries(umicom-toolchain-environment-admission-test PRIVATE Umicom::toolchain)
    set_target_properties(umicom-toolchain-environment-admission-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-toolchain-environment-admission-test)
    umicom_apply_sanitizers(umicom-toolchain-environment-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-toolchain-environment-admission-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-toolchain-environment-admission-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/environment.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/profile.c"
            "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/toolchain/environment.h")
    endif()
    foreach(_case valid missing-optional missing-c declared-count
            profile-size profile-version profile-count bin-unterminated prefix-unterminated
            c-unterminated cpp-unterminated cpp-newline pkg-newline ninja-newline rc-newline
            prefix-newline java-unterminated java-newline java-parent-first java-parent-second
            allocation path-long path-unset path-empty)
        add_test(NAME "framework.toolchain.environment.profile.${_case}"
            COMMAND umicom-toolchain-environment-admission-test "${_case}")
        set_tests_properties("framework.toolchain.environment.profile.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;toolchain;environment;regression;no-process")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.toolchain.environment.profile.${_case}"
                TARGETS umicom-toolchain-environment-admission-test)
        endif()
    endforeach()
endif()

# Cache interpretation and recovery share the native toolchain service.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCacheRecoveryChecks.cmake")

# Preset creation reuses Protocol JSON and exclusive Platform output.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomUserPresetChecks.cmake")

# Clean uses shared cache admission and native non-following tree removal.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTreeRemovalChecks.cmake")

# Pure metadata preflight protects cleanup and log reservation for every host.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomBuildPreflightChecks.cmake")
