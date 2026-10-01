# Debugger source navigation is a UI composition, keeping document ownership
# out of the core debugger model and available to installed Framework consumers.
add_library(umicom_debug_ui STATIC "${CMAKE_CURRENT_LIST_DIR}/../src/debug_ui/navigation.c")
add_library(Umicom::debug_ui ALIAS umicom_debug_ui)
set_target_properties(umicom_debug_ui PROPERTIES EXPORT_NAME debug_ui)
target_link_libraries(umicom_debug_ui PUBLIC Umicom::debug Umicom::diagnostic_ui)
target_include_directories(umicom_debug_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
umicom_apply_warnings(umicom_debug_ui)
umicom_apply_sanitizers(umicom_debug_ui)
target_link_libraries(umicom_framework INTERFACE Umicom::debug_ui)
install(TARGETS umicom_debug_ui EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
if(BUILD_TESTING)
    function(umicom_debug_selection_cases group file)
        set(target "umicom-debug-selection-${group}-test")
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/debug_selection/${file}")
        target_link_libraries(${target} PRIVATE Umicom::debug_ui)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        foreach(case IN LISTS ARGN)
            add_test(NAME "framework.debug_selection.${group}.${case}" COMMAND ${target} "${case}")
            set_tests_properties("framework.debug_selection.${group}.${case}" PROPERTIES
                TIMEOUT 30 LABELS "framework;debugger;regression")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endfunction()
    umicom_debug_selection_cases(capture test_capture.c
        ownership owner-reuse replace reuse running thread selection configuration session
        controller unrelated invalid repair-frame repair-thread repair-scope orphan)
    umicom_debug_selection_cases(open test_open.c
        absolute relative draft file-uri missing-file missing-line explicit-base invalid)
endif()

# Reuse the existing deterministic peer when the development workflow is built.
if(BUILD_TESTING AND TARGET umicom-dap-protocol-fixture)
    add_executable(umicom-debug-launch-base-test "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_selection/test_launch_base.c")
    target_link_libraries(umicom-debug-launch-base-test PRIVATE Umicom::Framework)
    umicom_apply_warnings(umicom-debug-launch-base-test)
    umicom_apply_sanitizers(umicom-debug-launch-base-test)
    add_dependencies(umicom-debug-launch-base-test umicom-dap-protocol-fixture)
    add_test(NAME framework.debug_selection.native_launch_base COMMAND umicom-debug-launch-base-test
        "$<TARGET_FILE:umicom-dap-protocol-fixture>")
    set_tests_properties(framework.debug_selection.native_launch_base PROPERTIES
        TIMEOUT 20 LABELS "framework;debugger;process;regression")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-debug-launch-base-test)
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/DEBUGGER_SOURCE_NAVIGATION.md"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom-framework/docs)
