# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Device fixtures have one owner even if the education CLI is added later.
get_property(_device_registered GLOBAL PROPERTY UMI_EDUCATION_DEVICE_IO_TESTS)
if(NOT _device_registered)
    umicom_education_inspection_test(umicom-education-device-io-trace-test test_device_io_trace.c)
    add_test(NAME framework.education.device_io_trace
        COMMAND umicom-education-device-io-trace-test
            "${_umicom_inspection_fixture_dir}/device I O trace.bin")
    set_tests_properties(framework.education.device_io_trace PROPERTIES
        FIXTURES_SETUP education_device_io_fixture TIMEOUT 30
        LABELS "education;framework;native;safe-c")
    # A successful parse must not publish its result if closing input fails.
    add_executable(umicom-education-device-io-close-test
        "${_umicom_inspection_test_root}/tests/education/test_trace_file_close.c")
    target_include_directories(umicom-education-device-io-close-test PRIVATE
        "${_umicom_inspection_test_root}/include")
    target_compile_definitions(umicom-education-device-io-close-test PRIVATE UMICOM_TRACE_DEVICE_IO=1)
    umicom_education_inspection_configure_target(umicom-education-device-io-close-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-education-device-io-close-test)
    endif()
    add_test(NAME framework.education.device_io_trace.close_failure
        COMMAND umicom-education-device-io-close-test
            "${_umicom_inspection_fixture_dir}/device close failure.bin")
    set_tests_properties(framework.education.device_io_trace.close_failure PROPERTIES
        TIMEOUT 30 LABELS "framework;education;io;regression")
    add_test(NAME framework.education.inspection.example.device_io
        COMMAND umicom-education-inspect-example device-io-trace
            "${_umicom_inspection_fixture_dir}/device I O trace.bin")
    set_tests_properties(framework.education.inspection.example.device_io PROPERTIES
        FIXTURES_REQUIRED education_device_io_fixture TIMEOUT 30
        PASS_REGULAR_EXPRESSION "Device I/O records:"
        LABELS "education;framework;example;sdk")
    set_property(GLOBAL PROPERTY UMI_EDUCATION_DEVICE_IO_TESTS TRUE)
endif()
get_property(_device_cli_registered GLOBAL PROPERTY UMI_EDUCATION_DEVICE_IO_CLI)
if(TARGET umicom-education AND NOT _device_cli_registered)
    add_test(NAME framework.education.device_io_trace.cli
        COMMAND umicom-education device-io-trace --file
            "${_umicom_inspection_fixture_dir}/device I O trace.bin")
    set_tests_properties(framework.education.device_io_trace.cli PROPERTIES
        FIXTURES_REQUIRED education_device_io_fixture TIMEOUT 30
        PASS_REGULAR_EXPRESSION "Device I/O trace:"
        LABELS "education;framework;native;safe-c")
    set_property(GLOBAL PROPERTY UMI_EDUCATION_DEVICE_IO_CLI TRUE)
endif()
