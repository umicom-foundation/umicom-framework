# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Running-process attachment is a shared debugger operation, independent of project builds.
include_guard(GLOBAL)
target_sources(umicom_developer PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/native_attach_plan.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/ATTACH_RUNNING_APPLICATION.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs" COMPONENT Framework)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_native_attach_test target source)
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/native_attach/${source}")
    target_link_libraries("${target}" PRIVATE Umicom::developer)
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${target}")
    endif()
endfunction()
umicom_native_attach_test(umicom-native-attach-id-test test_process_id.c)
umicom_native_attach_test(umicom-native-attach-plan-test test_plan.c)
foreach(case ordinary minimum maximum leading-zero overflow zero empty space trailing sign negative hex newline long nul)
    add_test(NAME framework.debug.attach.process_id.${case} COMMAND umicom-native-attach-id-test "${case}")
    set_tests_properties(framework.debug.attach.process_id.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;debug;attach;validation")
endforeach()
foreach(case gdb lldb symbols builtin zero overflow kind relative-root relative-program relative-adapter relative-tools directory-program file-root long-root utf8)
    add_test(NAME framework.debug.attach.plan.${case}
        COMMAND umicom-native-attach-plan-test "${case}" "$<TARGET_FILE:umicom-native-attach-plan-test>")
    set_tests_properties(framework.debug.attach.plan.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;debug;attach;validation")
endforeach()
if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    umicom_native_attach_test(umicom-native-attach-adapter-fixture adapter_fixture.c)
    umicom_native_attach_test(umicom-native-attach-protocol-test test_native.c)
    add_dependencies(umicom-native-attach-protocol-test umicom-native-attach-adapter-fixture)
    foreach(case normal lldb deferred destroy busy self refuse timeout)
        set(directory "${CMAKE_CURRENT_BINARY_DIR}/native-attach/${case}")
        file(MAKE_DIRECTORY "${directory}")
        add_test(NAME framework.debug.attach.protocol.${case}
            COMMAND umicom-native-attach-protocol-test "${case}"
                "$<TARGET_FILE:umicom-native-attach-adapter-fixture>" "${directory}")
        set_tests_properties(framework.debug.attach.protocol.${case} PROPERTIES
            TIMEOUT 60 LABELS "framework;debug;attach;native-transport;fixture")
    endforeach()
endif()
