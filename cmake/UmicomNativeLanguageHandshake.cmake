# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-native-language-handshake "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_native_handshake.c")
    target_link_libraries(umicom-native-language-handshake PRIVATE Umicom::developer Umicom::platform)
    umicom_apply_warnings(umicom-native-language-handshake)
    umicom_apply_sanitizers(umicom-native-language-handshake)
    if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(umicom-native-language-handshake PRIVATE -municode)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-native-language-handshake)
    endif()
    add_dependencies(umicom-native-language-handshake umicom-language-process-fixture)
    foreach(case valid unicode configured reuse stopped timeout error invalid probe probe-error probe-timeout probe-cancel probe-invalid)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/native-language/${case}")
        add_test(NAME framework.language_runtime.native_handshake.${case}
            COMMAND umicom-native-language-handshake ${case} "$<TARGET_FILE:umicom-language-process-fixture>")
        set_tests_properties(framework.language_runtime.native_handshake.${case} PROPERTIES TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/native-language/${case}"
            LABELS "framework;language;native;lifecycle;regression")
    endforeach()
endif()
