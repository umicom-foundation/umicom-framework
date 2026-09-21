# Umicom Framework | Sammy Hegab | Umicom Foundation | MIT
# Extend the preceding focused project; production sources are not copied here.
set(json_sources
    "${framework_root}/src/test_platform/ctest.c"
    "${framework_root}/src/test_platform/item.c"
    "${framework_root}/src/test_platform/suite.c"
    "${framework_root}/src/test_platform/discovery.c")
add_library(umicom-ctest-json-core STATIC ${json_sources})
target_include_directories(umicom-ctest-json-core PUBLIC "${framework_root}/include")
add_library(umicom-ctest-json-support STATIC json_test_support.c)
target_include_directories(umicom-ctest-json-support PUBLIC "${framework_root}/include")
foreach(kind json json_native process_stream)
    add_executable(umicom-ctest-${kind}-checks "test_${kind}.c")
    target_link_libraries(umicom-ctest-${kind}-checks PRIVATE umicom-ctest-json-support
        umicom-ctest-json-core umicom-ctest-checks-core umicom-ctest-checks-process
        umicom-ctest-checks-core)
    add_dependencies(umicom-ctest-checks umicom-ctest-${kind}-checks)
endforeach()
add_library(umicom-ctest-json-fault-core STATIC ${json_sources})
target_include_directories(umicom-ctest-json-fault-core PUBLIC "${framework_root}/include")
if(MSVC)
    target_compile_options(umicom-ctest-json-fault-core PRIVATE "/FI${CMAKE_CURRENT_LIST_DIR}/json_alloc_injection.h")
else()
    target_compile_options(umicom-ctest-json-fault-core PRIVATE -include "${CMAKE_CURRENT_LIST_DIR}/json_alloc_injection.h")
endif()
add_executable(umicom-ctest-json_allocation-checks test_json_allocation.c)
target_link_libraries(umicom-ctest-json_allocation-checks PRIVATE umicom-ctest-json-support
    umicom-ctest-json-fault-core umicom-ctest-checks-process umicom-ctest-checks-core)
add_dependencies(umicom-ctest-checks umicom-ctest-json_allocation-checks)
if(BUILD_TESTING)
    set(json_scenarios
        minimal
        empty
        metadata
        unicode
        spaces-punctuation
        disabled-literal
        name-255
        name-256
        command-overflow
        labels-overflow
        directory-overflow
        timeout-decimal
        timeout-fraction
        timeout-submillisecond
        timeout-zero
        timeout-max
        timeout-exponent
        timeout-negative
        timeout-overflow
        timeout-string
        timeout-boolean
        timeout-huge-positive
        timeout-huge-negative
        disabled-string
        disabled-number
        disabled-null
        wrong-config
        empty-name
        missing-name
        null-name
        name-nul
        labels-type
        properties-type
        command-type
        duplicate-property
        unknown-property
        duplicate-test
        later-invalid
        wrong-kind
        new-version
        new-minor
        missing-version
        tests-object
        trailing-comma
        duplicate-tests
        duplicate-name
        duplicate-kind
        trailing-document
        truncated
        lone-high-surrogate
        lone-low-surrogate
        wrong-surrogate-pair
        raw-nul
        depth-limit
        node-index
        node-negative
        file-index
        source-line-zero
        raw-utf8
        invalid-utf8
        refresh compatibility collision truncations)
    foreach(scenario IN LISTS json_scenarios)
        add_test(NAME "framework.ctest.json.${scenario}" COMMAND umicom-ctest-json-checks "${scenario}")
        set_tests_properties("framework.ctest.json.${scenario}" PROPERTIES TIMEOUT 60)
    endforeach()
    add_test(NAME framework.ctest.json.allocation COMMAND umicom-ctest-json_allocation-checks)
    add_test(NAME framework.ctest.json.raw-stream COMMAND umicom-ctest-process_stream-checks "$<TARGET_FILE:umicom-ctest-fixture-helper>")
    foreach(kind large empty)
        set(count 10000)
        if(kind STREQUAL "empty")
            set(count 0)
        endif()
        set(json_${kind}_build "${CMAKE_CURRENT_BINARY_DIR}/json ${kind} $<CONFIG>")
        add_test(NAME "framework.ctest.json_setup.${kind}" COMMAND "${CMAKE_COMMAND}"
            -S "${CMAKE_CURRENT_LIST_DIR}/json_fixture" -B "${json_${kind}_build}" "-DCASE_COUNT=${count}")
        set_tests_properties("framework.ctest.json_setup.${kind}" PROPERTIES FIXTURES_SETUP "json_${kind}" TIMEOUT 120)
    endforeach()
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/json missing")
    # A deliberately slow CTest include exercises the real process timeout.
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/json slow")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/json slow/CTestTestfile.cmake"
        "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E sleep 0.2)\n")
    foreach(scenario large empty legacy-entry capture-limit record-limit cancel-before timeout missing-root invalid-limit execution-bridge)
        set(dir "${json_large_build}")
        set(fixture json_large)
        if(scenario STREQUAL "empty")
            set(dir "${json_empty_build}")
            set(fixture json_empty)
        elseif(scenario STREQUAL "missing-root")
            set(dir "${CMAKE_CURRENT_BINARY_DIR}/json missing")
        elseif(scenario STREQUAL "timeout")
            set(dir "${CMAKE_CURRENT_BINARY_DIR}/json slow")
        endif()
        add_test(NAME "framework.ctest.json_native.${scenario}" COMMAND umicom-ctest-json_native-checks "${dir}" "${scenario}")
        set_tests_properties("framework.ctest.json_native.${scenario}" PROPERTIES FIXTURES_REQUIRED "${fixture}"
            RESOURCE_LOCK umicom_json_fixture TIMEOUT 120)
    endforeach()
    set_tests_properties(framework.ctest.json.allocation framework.ctest.json.raw-stream PROPERTIES TIMEOUT 120)
endif()

add_executable(umicom-ctest-json_process-checks test_json_process.c)
target_link_libraries(umicom-ctest-json_process-checks PRIVATE umicom-ctest-json-support
    umicom-ctest-json-core umicom-ctest-checks-core)
add_executable(umicom-ctest-json_registry-checks test_json_registry.c)
target_link_libraries(umicom-ctest-json_registry-checks PRIVATE umicom-ctest-json-support
    umicom-ctest-json-core umicom-ctest-checks-process umicom-ctest-checks-core)
add_dependencies(umicom-ctest-checks umicom-ctest-json_process-checks umicom-ctest-json_registry-checks)
if(BUILD_TESTING)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/json injected")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/json injected/CTestTestfile.cmake" "# Injected process boundary only.\n")
    foreach(scenario chunked stderr-prefix stderr-suffix empty-output partial-output exit-failure
            missing-launch cancelled timed-out process-error diagnostic-truncation byte-limit)
        add_test(NAME "framework.ctest.json_process.${scenario}" COMMAND umicom-ctest-json_process-checks
            "${scenario}" "${CMAKE_CURRENT_BINARY_DIR}/json injected")
        set_tests_properties("framework.ctest.json_process.${scenario}" PROPERTIES TIMEOUT 60)
    endforeach()
    add_test(NAME framework.ctest.json.registry COMMAND umicom-ctest-json_registry-checks)
    set_tests_properties(framework.ctest.json.registry PROPERTIES TIMEOUT 120)
endif()
