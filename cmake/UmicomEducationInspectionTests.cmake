#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomEducationInspectionTests.cmake
# PURPOSE: Register the same inspection coverage in SDK and standalone builds.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
if(NOT BUILD_TESTING)
    return()
endif()
if(NOT TARGET Umicom::education_inspection)
    message(FATAL_ERROR "Education inspection tests require their canonical library")
endif()
get_filename_component(_umicom_inspection_test_root
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
# Framework and the standalone command may be composed in either order.
# Register shared executables and fixtures once, then let the later CLI add its
# own checks in its directory without redefining the production targets.
get_property(_inspection_tests_registered GLOBAL PROPERTY UMI_EDUCATION_INSPECTION_TESTS_REGISTERED)
if(NOT _inspection_tests_registered)
    set(_umicom_inspection_fixture_dir
        "${CMAKE_CURRENT_BINARY_DIR}/education-inspection-tests/fixture with spaces")
    file(MAKE_DIRECTORY "${_umicom_inspection_fixture_dir}")

    # Every normal parser test links the production owner. Only the explicit fault
    # test compiles the implementation with local I/O substitutions.
    function(umicom_education_inspection_test target source)
        add_executable(${target} "${_umicom_inspection_test_root}/tests/education/${source}")
        target_link_libraries(${target} PRIVATE Umicom::education_inspection)
        umicom_education_inspection_configure_target(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endfunction()
    umicom_education_inspection_test(umicom-education-binary-test test_binary_inspect.c)
    umicom_education_inspection_test(umicom-education-archive-test test_archive_inspect.c)
    umicom_education_inspection_test(umicom-education-elf-test test_elf_symbols.c)
    umicom_education_inspection_test(umicom-education-load-plan-test test_elf_load_plan.c)
    umicom_education_inspection_test(umicom-education-memory-audit-test test_memory_audit.c)
    umicom_education_inspection_test(umicom-education-ipc-trace-test test_ipc_trace.c)

    add_executable(umicom-education-binary-file-test
        "${_umicom_inspection_test_root}/tests/education/test_binary_file_failures.c")
    target_include_directories(umicom-education-binary-file-test PRIVATE
        "${_umicom_inspection_test_root}/include")
    umicom_education_inspection_configure_target(umicom-education-binary-file-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-education-binary-file-test)
    endif()

    # A compiler-produced archive supplements synthetic data; CTest fixtures order
    # file producers before CLI/example consumers, including parallel test runs.
    add_library(umicom-education-archive-fixture STATIC
        "${_umicom_inspection_test_root}/tests/education/archive_fixture.c")
    umicom_education_inspection_configure_target(umicom-education-archive-fixture)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-education-archive-fixture)
    endif()
    set(_education_elf_fixture "${_umicom_inspection_fixture_dir}/umicom example.o")
    set(_education_load_plan_fixture "${_umicom_inspection_fixture_dir}/umicom kernel.elf")
    set(_education_ipc_fixture "${_umicom_inspection_fixture_dir}/owned messages.ipc")

    add_test(NAME framework.education.binary_inspect
        COMMAND umicom-education-binary-test "$<TARGET_FILE:umicom-education-binary-test>"
            "${_umicom_inspection_fixture_dir}/sample.obj")
    add_test(NAME framework.education.binary_file_failures
        COMMAND umicom-education-binary-file-test)
    add_test(NAME framework.education.archive_inspect
        COMMAND umicom-education-archive-test "${_umicom_inspection_fixture_dir}/lesson sample.a")
    add_test(NAME framework.education.elf_symbols
        COMMAND umicom-education-elf-test "${_education_elf_fixture}")
    add_test(NAME framework.education.elf_load_plan
        COMMAND umicom-education-load-plan-test "${_education_load_plan_fixture}")
    add_test(NAME framework.education.memory_audit COMMAND umicom-education-memory-audit-test)
    add_test(NAME framework.education.ipc_trace
        COMMAND umicom-education-ipc-trace-test "${_education_ipc_fixture}")
    set_tests_properties(framework.education.elf_symbols PROPERTIES
        FIXTURES_SETUP education_elf_fixture)
    set_tests_properties(framework.education.elf_load_plan PROPERTIES
        FIXTURES_SETUP education_load_plan_fixture)
    set_tests_properties(framework.education.ipc_trace PROPERTIES
        FIXTURES_SETUP education_ipc_trace_fixture)
    set_tests_properties(
        framework.education.binary_inspect framework.education.binary_file_failures
        framework.education.archive_inspect framework.education.elf_symbols
        framework.education.elf_load_plan framework.education.memory_audit
        framework.education.ipc_trace
        PROPERTIES TIMEOUT 120 LABELS "education;framework;native;safe-c")

    # The public example is a thin SDK consumer, also compiled against the live
    # target here. Its installed build is checked separately using a copied prefix.
    add_subdirectory("${_umicom_inspection_test_root}/examples/education_inspection"
        "${CMAKE_CURRENT_BINARY_DIR}/education-inspection-example")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-education-inspect-example)
    endif()
    add_test(NAME framework.education.inspection.example.binary
        COMMAND umicom-education-inspect-example binary "$<TARGET_FILE:umicom-education-inspect-example>")
    add_test(NAME framework.education.inspection.example.archive
        COMMAND umicom-education-inspect-example archive "$<TARGET_FILE:umicom-education-archive-fixture>")
    add_test(NAME framework.education.inspection.example.symbols
        COMMAND umicom-education-inspect-example symbols "${_education_elf_fixture}")
    add_test(NAME framework.education.inspection.example.load_plan
        COMMAND umicom-education-inspect-example load-plan "${_education_load_plan_fixture}")
    add_test(NAME framework.education.inspection.example.memory
        COMMAND umicom-education-inspect-example memory-audit "${_education_load_plan_fixture}")
    add_test(NAME framework.education.inspection.example.ipc
        COMMAND umicom-education-inspect-example ipc-trace "${_education_ipc_fixture}")
    set_tests_properties(framework.education.inspection.example.binary PROPERTIES
        PASS_REGULAR_EXPRESSION "Format: (ELF|PE)")
    set_tests_properties(framework.education.inspection.example.archive PROPERTIES
        PASS_REGULAR_EXPRESSION "Archive members:")
    set_tests_properties(framework.education.inspection.example.symbols PROPERTIES
        FIXTURES_REQUIRED education_elf_fixture PASS_REGULAR_EXPRESSION "ELF symbols:")
    set_tests_properties(framework.education.inspection.example.load_plan PROPERTIES
        FIXTURES_REQUIRED education_load_plan_fixture PASS_REGULAR_EXPRESSION "Load segments:")
    set_tests_properties(framework.education.inspection.example.memory PROPERTIES
        FIXTURES_REQUIRED education_load_plan_fixture PASS_REGULAR_EXPRESSION "Writable executable pages: 0")
    set_tests_properties(framework.education.inspection.example.ipc PROPERTIES
        FIXTURES_REQUIRED education_ipc_trace_fixture PASS_REGULAR_EXPRESSION "IPC records: 4")
    set_tests_properties(
        framework.education.inspection.example.binary framework.education.inspection.example.archive
        framework.education.inspection.example.symbols framework.education.inspection.example.load_plan
        framework.education.inspection.example.memory framework.education.inspection.example.ipc
        PROPERTIES TIMEOUT 30 LABELS "education;framework;example;sdk")

    # Pass generator/toolchain choices explicitly to the nested package check.
    # It installs only this component into its own build-tree prefix, never into
    # an existing user SDK, and does not run cross-compiled executables on the host.
    if(NOT CMAKE_CROSSCOMPILING)
        add_test(NAME framework.education.inspection.installed_consumer
            COMMAND "${CMAKE_COMMAND}"
                "-DFRAMEWORK_ROOT=${_umicom_inspection_test_root}"
                "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/education-inspection-sdk"
                "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
                "-DTEST_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
                "-DTEST_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
                "-DTEST_COMPILER=${CMAKE_C_COMPILER}"
                "-DTEST_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
                "-DTEST_TOOLCHAIN=${CMAKE_TOOLCHAIN_FILE}"
                "-DTEST_CONFIG=$<CONFIG>"
                -P "${_umicom_inspection_test_root}/tests/education/check_installed_inspection.cmake")
        set_tests_properties(framework.education.inspection.installed_consumer PROPERTIES
            TIMEOUT 240 LABELS "education;framework;sdk;installed-consumer")
    endif()

    set_property(GLOBAL PROPERTY UMI_EDUCATION_INSPECTION_TESTS_REGISTERED TRUE)
    set_property(GLOBAL PROPERTY UMI_EDUCATION_INSPECTION_FIXTURE_DIRECTORY
        "${_umicom_inspection_fixture_dir}")
endif()

# The CLI may be added by a later subdirectory. Retrieve the original fixture
# location instead of assuming that its current binary directory is the same.
get_property(_umicom_inspection_fixture_dir GLOBAL PROPERTY UMI_EDUCATION_INSPECTION_FIXTURE_DIRECTORY)
set(_education_elf_fixture "${_umicom_inspection_fixture_dir}/umicom example.o")
set(_education_load_plan_fixture "${_umicom_inspection_fixture_dir}/umicom kernel.elf")
set(_education_ipc_fixture "${_umicom_inspection_fixture_dir}/owned messages.ipc")
get_property(_inspection_cli_registered GLOBAL PROPERTY UMI_EDUCATION_INSPECTION_CLI_REGISTERED)
# Preserve the existing standalone CLI checks and their public CTest names.
# A normal SDK build has no site-serving CLI and still runs every parser test.
if(TARGET umicom-education AND NOT _inspection_cli_registered)
    add_test(NAME framework.education.binary.cli
        COMMAND umicom-education binary --file "$<TARGET_FILE:umicom-education>")
    set_tests_properties(framework.education.binary.cli PROPERTIES
        PASS_REGULAR_EXPRESSION "Format: (ELF|PE)")
    add_test(NAME framework.education.archive.cli
        COMMAND umicom-education archive --file "$<TARGET_FILE:umicom-education-archive-fixture>")
    set_tests_properties(framework.education.archive.cli PROPERTIES
        PASS_REGULAR_EXPRESSION "Format: ar")
    add_test(NAME framework.education.symbols.cli
        COMMAND umicom-education symbols --file "${_education_elf_fixture}")
    set_tests_properties(framework.education.symbols.cli PROPERTIES
        FIXTURES_REQUIRED education_elf_fixture PASS_REGULAR_EXPRESSION "Format: ELF64")
    add_test(NAME framework.education.load_plan.cli
        COMMAND umicom-education load-plan --file "${_education_load_plan_fixture}")
    set_tests_properties(framework.education.load_plan.cli PROPERTIES
        FIXTURES_REQUIRED education_load_plan_fixture
        PASS_REGULAR_EXPRESSION "ELF load plan: ELF64")
    add_test(NAME framework.education.memory_audit.cli
        COMMAND umicom-education memory-audit --file "${_education_load_plan_fixture}")
    set_tests_properties(framework.education.memory_audit.cli PROPERTIES
        FIXTURES_REQUIRED education_load_plan_fixture
        PASS_REGULAR_EXPRESSION "Writable and executable pages: 0")
    add_test(NAME framework.education.ipc_trace.cli
        COMMAND umicom-education ipc-trace --file "${_education_ipc_fixture}")
    set_tests_properties(framework.education.ipc_trace.cli PROPERTIES
        FIXTURES_REQUIRED education_ipc_trace_fixture
        PASS_REGULAR_EXPRESSION "IPC trace: 4 records")
    set_tests_properties(
        framework.education.binary.cli framework.education.archive.cli
        framework.education.symbols.cli framework.education.load_plan.cli
        framework.education.memory_audit.cli framework.education.ipc_trace.cli
        PROPERTIES TIMEOUT 120 LABELS "education;framework;native;safe-c")
    set_property(GLOBAL PROPERTY UMI_EDUCATION_INSPECTION_CLI_REGISTERED TRUE)
endif()


# Classroom scheduling fixtures also belong to the shared inspection owner.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomEducationSchedulingTests.cmake")
