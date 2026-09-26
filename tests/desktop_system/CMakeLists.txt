# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-desktop-system-test test_monitor.c)
target_link_libraries(umicom-desktop-system-test PRIVATE Umicom::desktop_system)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-desktop-system-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-desktop-system-test)
endif()
foreach(_case IN ITEMS
    memory-valid
    memory-missing
    memory-duplicate
    memory-units
    memory-range
    memory-overflow
    memory-negative
    memory-zero
    cpu-guest
    cpu-bad
    cpu-ratio
    cpu-extreme
    cpu-reset
    cpu-zero
    cpu-overflow
    process-valid
    process-controls
    process-overflow
    process-negative
    process-short
    process-nul
    network-valid
    network-bad
    json-valid
    json-capacity
    json-invalid
    session-wayland
    session-x11
    session-path
    session-socket
    session-remote-x11
    session-remote-bus
    session-injection
    session-missing-display
    invalid-capture)
    add_test(NAME framework.desktop_system.${_case} COMMAND umicom-desktop-system-test ${_case})
    set_tests_properties(framework.desktop_system.${_case} PROPERTIES TIMEOUT 30
        LABELS "framework;desktop-system;regression")
endforeach()
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND AND TARGET umicom-system-report)
    add_test(NAME framework.desktop_system.independent
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/test_independent.py"
            "$<TARGET_FILE:umicom-desktop-system-test>" "$<TARGET_FILE:umicom-system-report>")
    set_tests_properties(framework.desktop_system.independent PROPERTIES TIMEOUT 60
        LABELS "framework;desktop-system;independent")
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND Python3_Interpreter_FOUND AND TARGET umicom-desk-session)
    add_executable(umicom-desktop-session-child session_child.c)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-desktop-session-child)
    endif()
    # The child checks post-exec descriptor/environment state and is purposely
    # not linked to ASan, whose extra descriptors would alter that evidence.
    add_test(NAME framework.desktop_system.session-exec
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/test_session_exec.py"
            "$<TARGET_FILE:umicom-desk-session>" "$<TARGET_FILE:umicom-desktop-session-child>")
    set_tests_properties(framework.desktop_system.session-exec PROPERTIES TIMEOUT 60 SKIP_RETURN_CODE 77
        LABELS "framework;desktop-system;linux;process-replacement")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-desktop-system-test)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND Python3_Interpreter_FOUND AND TARGET umicom-desk-session)
    add_test(NAME framework.desktop_system.stage
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/test_stage.py"
            "${CMAKE_CURRENT_LIST_DIR}/../../scripts/stage-desktop-session.py"
            "$<TARGET_FILE:umicom-desk-session>" "$<TARGET_FILE:umicom-desktop-session-child>")
    set_tests_properties(framework.desktop_system.stage PROPERTIES TIMEOUT 60
        LABELS "framework;desktop-system;packaging;regression")
endif()

add_executable(umicom-desktop-system-adversarial-test test_adversarial.c)
target_link_libraries(umicom-desktop-system-adversarial-test PRIVATE Umicom::desktop_system)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-desktop-system-adversarial-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-desktop-system-adversarial-test)
endif()
add_test(NAME framework.desktop_system.parser-adversarial COMMAND umicom-desktop-system-adversarial-test)
set_tests_properties(framework.desktop_system.parser-adversarial PROPERTIES TIMEOUT 30
    LABELS "framework;desktop-system;parser;regression")
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-desktop-system-adversarial-test)
endif()
