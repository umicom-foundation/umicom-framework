# Umicom Framework
# PURPOSE: Share scheduling trace fixtures between SDK and education command.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# The common inspection module defines the target helper and fixture directory.
# Separate registration flags also support a CLI added after the Framework SDK.
get_property(_scheduling_registered GLOBAL PROPERTY UMI_EDUCATION_SCHEDULING_TESTS)
if(NOT _scheduling_registered)
    foreach(kind IN ITEMS schedule timer interrupt evidence)
        umicom_education_inspection_test(umicom-education-${kind}-trace-test
            test_${kind}_trace.c)
        add_test(NAME framework.education.${kind}_trace
            COMMAND umicom-education-${kind}-trace-test
                "${_umicom_inspection_fixture_dir}/${kind} trace.bin")
        set_tests_properties(framework.education.${kind}_trace PROPERTIES
            FIXTURES_SETUP education_${kind}_trace_fixture
            TIMEOUT 30 LABELS "education;framework;native;safe-c")
    endforeach()
    # Compile each production wrapper with only fclose interposed. Keep these
    # separate from the library to avoid duplicate public definitions.
    foreach(kind IN ITEMS schedule timer interrupt evidence)
        add_executable(umicom-education-${kind}-close-test
            "${_umicom_inspection_test_root}/tests/education/test_trace_file_close.c")
        target_include_directories(umicom-education-${kind}-close-test PRIVATE
            "${_umicom_inspection_test_root}/include")
        if(kind STREQUAL "timer")
            target_compile_definitions(umicom-education-${kind}-close-test PRIVATE UMICOM_TRACE_TIMER=1)
        elseif(kind STREQUAL "interrupt")
            target_compile_definitions(umicom-education-${kind}-close-test PRIVATE UMICOM_TRACE_INTERRUPT=1)
        elseif(kind STREQUAL "evidence")
            target_compile_definitions(umicom-education-${kind}-close-test PRIVATE UMICOM_TRACE_EVIDENCE=1)
            target_sources(umicom-education-${kind}-close-test PRIVATE
                "${_umicom_inspection_test_root}/src/base/sha256.c")
        endif()
        umicom_education_inspection_configure_target(umicom-education-${kind}-close-test)
        add_test(NAME framework.education.${kind}_trace.close_failure
            COMMAND umicom-education-${kind}-close-test
                "${_umicom_inspection_fixture_dir}/${kind} close failure.bin")
        set_tests_properties(framework.education.${kind}_trace.close_failure PROPERTIES
            TIMEOUT 30 LABELS "framework;education;io;regression")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-education-${kind}-close-test)
        endif()
    endforeach()
    # Exercise the installed-style example against the same deterministic files.
    foreach(kind IN ITEMS schedule timer interrupt evidence)
        add_test(NAME framework.education.inspection.example.${kind}
            COMMAND umicom-education-inspect-example ${kind}-trace
                "${_umicom_inspection_fixture_dir}/${kind} trace.bin")
        set_tests_properties(framework.education.inspection.example.${kind} PROPERTIES
            FIXTURES_REQUIRED education_${kind}_trace_fixture
            TIMEOUT 30 LABELS "education;framework;example;sdk")
    endforeach()
    set_property(GLOBAL PROPERTY UMI_EDUCATION_SCHEDULING_TESTS TRUE)
endif()
get_property(_scheduling_cli_registered GLOBAL PROPERTY UMI_EDUCATION_SCHEDULING_CLI)
if(TARGET umicom-education AND NOT _scheduling_cli_registered)
    foreach(kind IN ITEMS schedule timer interrupt evidence)
        add_test(NAME framework.education.${kind}_trace.cli
            COMMAND umicom-education ${kind}-trace --file
                "${_umicom_inspection_fixture_dir}/${kind} trace.bin")
        set_tests_properties(framework.education.${kind}_trace.cli PROPERTIES
            FIXTURES_REQUIRED education_${kind}_trace_fixture
            TIMEOUT 30 LABELS "education;framework;native;safe-c")
    endforeach()
    set_tests_properties(framework.education.schedule_trace.cli PROPERTIES
        PASS_REGULAR_EXPRESSION "Scheduler trace: 11 records")
    set_tests_properties(framework.education.timer_trace.cli PROPERTIES
        PASS_REGULAR_EXPRESSION "Timer trace: 11 records")
    set_tests_properties(framework.education.interrupt_trace.cli PROPERTIES
        PASS_REGULAR_EXPRESSION "Interrupt trace: 20 records")
    set_tests_properties(framework.education.evidence_trace.cli PROPERTIES
        PASS_REGULAR_EXPRESSION "Evidence trace: 20 records")
    set_property(GLOBAL PROPERTY UMI_EDUCATION_SCHEDULING_CLI TRUE)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomEducationDeviceIoTests.cmake")
