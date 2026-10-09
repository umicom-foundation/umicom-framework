# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# These tests launch only the isolated probe, never a project compiler.
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-tool-location-test "${CMAKE_CURRENT_LIST_DIR}/../tests/tool_location/test_selection.c")
    add_executable(umicom-tool-process-test "${CMAKE_CURRENT_LIST_DIR}/../tests/tool_location/test_process.c")
    add_executable(umicom-tool-probe "${CMAKE_CURRENT_LIST_DIR}/../tests/tool_location/process_probe.c")
    target_link_libraries(umicom-tool-location-test PRIVATE Umicom::build)
    foreach(target umicom-tool-process-test umicom-tool-probe)
        target_link_libraries(${target} PRIVATE Umicom::platform)
    endforeach()
    set_target_properties(umicom-tool-probe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tool probe")
    foreach(target umicom-tool-location-test umicom-tool-process-test umicom-tool-probe)
        set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endforeach()
    foreach(case selection validation search-path)
        add_test(NAME framework.tool_location.${case} COMMAND umicom-tool-location-test "${case}")
        set_tests_properties(framework.tool_location.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;build;tool-location;regression")
    endforeach()
    foreach(case inherited override duplicate missing)
        add_test(NAME framework.tool_location.native.${case}
            COMMAND umicom-tool-process-test "${case}" "$<TARGET_FILE_DIR:umicom-tool-probe>")
        set_tests_properties(framework.tool_location.native.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;platform;tool-location;native;regression")
    endforeach()
endif()

# Persistent tools use the same selected installation through protocol pipes.
if(BUILD_TESTING)
    add_executable(umicom-tool-stream-test "${CMAKE_CURRENT_LIST_DIR}/../tests/tool_location/test_stream.c")
    target_link_libraries(umicom-tool-stream-test PRIVATE Umicom::developer Umicom::platform)
    umicom_apply_warnings(umicom-tool-stream-test)
    umicom_apply_sanitizers(umicom-tool-stream-test)
    add_dependencies(umicom-tool-stream-test umicom-tool-probe)
    add_dependencies(umicom-tool-process-test umicom-tool-probe)
    if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(umicom-tool-probe PRIVATE -municode)
        target_link_options(umicom-tool-stream-test PRIVATE -municode)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-tool-stream-test)
    endif()
    foreach(case selected absolute inherited relative-folder relative-program missing)
        add_test(NAME framework.tool_location.stream.${case}
            COMMAND umicom-tool-stream-test "${case}" "$<TARGET_FILE_DIR:umicom-tool-probe>"
                "$<TARGET_FILE:umicom-tool-probe>")
        set_tests_properties(framework.tool_location.stream.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;language;tool-location;native;regression")
    endforeach()
endif()

# File discovery checks use inert files and never launch the named tools.
if(BUILD_TESTING)
    add_executable(umicom-tool-discovery-test "${CMAKE_CURRENT_LIST_DIR}/../tests/tool_location/test_discovery.c")
    target_link_libraries(umicom-tool-discovery-test PRIVATE Umicom::toolchain Umicom::platform)
    umicom_apply_warnings(umicom-tool-discovery-test)
    umicom_apply_sanitizers(umicom-tool-discovery-test)
    if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(umicom-tool-discovery-test PRIVATE -municode)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-tool-discovery-test)
    endif()
    foreach(case absolute order unicode capacity missing snapshot)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/tool-discovery/${case}")
        add_test(NAME framework.tool_location.discovery.${case} COMMAND umicom-tool-discovery-test "${case}")
        set_tests_properties(framework.tool_location.discovery.${case} PROPERTIES
            TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/tool-discovery/${case}"
            LABELS "framework;toolchain;tool-location;filesystem;regression")
    endforeach()
endif()
