# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Exercise the same native process stream used by persistent language servers.
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-language-process-fixture "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/process_stream_fixture.c")
    add_executable(umicom-language-process-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_process_stream_native.c")
    target_link_libraries(umicom-language-process-test PRIVATE Umicom::developer Umicom::platform)
    foreach(target umicom-language-process-fixture umicom-language-process-test)
        umicom_apply_warnings(${target})
        if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
            target_link_options(${target} PRIVATE -municode)
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endforeach()
    # The fixture is deliberately terminated in lifecycle cases. Instrument
    # the parent owner only, so its cleanup remains visible to sanitizers.
    umicom_apply_sanitizers(umicom-language-process-test)
    add_dependencies(umicom-language-process-test umicom-language-process-fixture)
    foreach(case invalid null-argument too-many missing-program missing-directory invalid-utf8 command-limit empty quotes unicode many long unicode-program cwd environment stderr-separate stderr-merged echo closed-input exit-active timeout stop destroy read-invalid eof)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/language-process/${case}")
        add_test(NAME framework.language_runtime.native_process.${case}
            COMMAND umicom-language-process-test ${case} "$<TARGET_FILE:umicom-language-process-fixture>")
        set_tests_properties(framework.language_runtime.native_process.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/language-process/${case}"
            LABELS "framework;language;native;process;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/native-language-processes.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
