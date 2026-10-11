# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Portable captures belong to the debugger owner; GTK only presents those values.
target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/instruction_fields.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/instruction_page.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/instruction_target.c")
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/debug_instructions.c")
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/DEBUG_INSTRUCTIONS.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs" COMPONENT Framework)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_instruction_test target source owner)
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/debug_instructions/${source}")
    target_link_libraries("${target}" PRIVATE "${owner}")
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${target}")
    endif()
endfunction()
function(umicom_instruction_case group target case)
    add_test(NAME framework.debug.instructions.${group}.${case} COMMAND "${target}" "${case}" ${ARGN})
    set_tests_properties(framework.debug.instructions.${group}.${case} PROPERTIES
        TIMEOUT 60 SKIP_RETURN_CODE 77 LABELS "framework;debug;instructions;regression")
endfunction()
umicom_instruction_test(umicom-instruction-decode-test test_decode.c Umicom::developer)
foreach(case normal wide decimal filler inherit unicode unknown-hint escaped-key max-line short long missing array-type row-type missing-address missing-text text-type address-expression empty-hex negative-address duplicate nul negative-line fraction large-line location-type reference-range limits offsets reference legacy addresses)
    umicom_instruction_case(decode umicom-instruction-decode-test "${case}")
endforeach()
if(TARGET umicom-native-attach-adapter-fixture)
    umicom_instruction_test(umicom-instruction-protocol-test test_protocol.c Umicom::developer)
    add_dependencies(umicom-instruction-protocol-test umicom-native-attach-adapter-fixture)
    foreach(case normal unsupported missing-reference changed-frame capture-refuse capture-timeout capture-event refuse timeout malformed continued short stale wrong-workspace wrong-service closed reconnect invalid-options paging small-stack stack-invalid)
        set(directory "${CMAKE_CURRENT_BINARY_DIR}/debug-instructions/${case}")
        file(MAKE_DIRECTORY "${directory}")
        umicom_instruction_case(protocol umicom-instruction-protocol-test "${case}"
            "$<TARGET_FILE:umicom-native-attach-adapter-fixture>" "${directory}")
    endforeach()
endif()
if(TARGET umicom_ui_gtk4)
    umicom_instruction_test(umicom-instruction-gtk4-test test_gtk4.c Umicom::ui_gtk4)
    foreach(case normal paging edited invalid-offset offset-limit denied read-denied hidden retained close-capture close-read edit-capture edit-read invalid-context invalid-page failed-refresh)
        umicom_instruction_case(gtk4 umicom-instruction-gtk4-test "${case}")
    endforeach()
endif()

# A malformed stack must not renumber later instruction captures.
umicom_instruction_test(umicom-instruction-stack-test test_stack.c Umicom::developer)
foreach(case zero ordered source empty partial duplicate-id duplicate-key id-type id-range name-type coordinate-range coordinate-negative source-type source-reference reference-type total-type total-short array-type capacity)
    umicom_instruction_case(stack umicom-instruction-stack-test "${case}")
endforeach()

# Protocol stack totals use unsigned bounds independently from frame identifiers.
foreach(case total-wide total-range)
    umicom_instruction_case(stack umicom-instruction-stack-test "${case}")
endforeach()

# Execution requests must invalidate captures even when the adapter reply is lost.
if(TARGET umicom-native-attach-adapter-fixture)
    umicom_instruction_test(umicom-instruction-control-test test_control.c Umicom::developer)
    add_dependencies(umicom-instruction-control-test umicom-native-attach-adapter-fixture)
    foreach(case continue next into out back reverse pause pause-ack timeout refuse wrong-command bad-body partial bodyless early-stop late-stop invalid duplicate)
        set(directory "${CMAKE_CURRENT_BINARY_DIR}/debug-control/${case}")
        file(MAKE_DIRECTORY "${directory}")
        umicom_instruction_case(control umicom-instruction-control-test "${case}"
            "$<TARGET_FILE:umicom-native-attach-adapter-fixture>" "${directory}")
    endforeach()
endif()
