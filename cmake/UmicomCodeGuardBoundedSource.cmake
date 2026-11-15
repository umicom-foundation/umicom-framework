#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardBoundedSource.cmake
#
# PURPOSE:
# Register bounded CodeGuard input and analysis qualification.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    foreach(_kind IN ITEMS reader source_native)
        add_executable(umicom-codeguard-bounded-${_kind}-tests
            "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_bounded_${_kind}.c")
        target_link_libraries(umicom-codeguard-bounded-${_kind}-tests PRIVATE Umicom::codeguard)
        target_compile_features(umicom-codeguard-bounded-${_kind}-tests PRIVATE c_std_23)
        umicom_apply_warnings(umicom-codeguard-bounded-${_kind}-tests)
        umicom_apply_sanitizers(umicom-codeguard-bounded-${_kind}-tests)
    endforeach()
    set(_reader_cases arguments relative empty exact over zero-over segments long newline-boundary crlf nul late-nul late-read late-change late-close cancel-before cancel-line cancel-empty failure-cancel callback-failure line-overflow measure measure-failure measure-alias hash hash-binary hash-failure hash-overflow missing trailing-slash)
    foreach(_case IN LISTS _reader_cases)
        add_test(NAME "framework.codeguard_bounded_reader.${_case}"
            COMMAND umicom-codeguard-bounded-reader-tests "${_case}")
        set_tests_properties("framework.codeguard_bounded_reader.${_case}" PROPERTIES
            TIMEOUT 20 LABELS "codeguard;bounded-input;fault-injection")
    endforeach()
    set(_root "${CMAKE_CURRENT_BINARY_DIR}/tests/codeguard-bounded-source")
    file(MAKE_DIRECTORY "${_root}/directory.c")
    # Native-newline file(WRITE) retained for review; exact budgets need LF.
    #file(WRITE "${_root}/ordinary.c" "a + b;\n")
    file(CONFIGURE OUTPUT "${_root}/ordinary.c" CONTENT "a + b;\n" NEWLINE_STYLE LF)
    # Native-newline file(WRITE) retained for review; exact budgets need LF.
    #file(WRITE "${_root}/模型.c" "a + b;\n")
    file(CONFIGURE OUTPUT "${_root}/模型.c" CONTENT "a + b;\n" NEWLINE_STYLE LF)
    file(WRITE "${_root}/empty.c" "")
    string(REPEAT "x" 8194 _long)
    file(WRITE "${_root}/long.c" "${_long}")
    # Place '/' at the last byte of the first native 64 KiB chunk, so the
    # following '*' begins the next chunk and must retain normalizer state.
    string(REPEAT " " 65534 _padding)
    file(WRITE "${_root}/boundary.c" "a${_padding}/*note*/+b")
    # CMake strings cannot contain NUL. Keep the three-byte binary fixture
    # in source and copy it without interpreting its contents.
    configure_file("${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/fixtures/binary.bin"
        "${_root}/binary.bin" COPYONLY)
    set(_native_cases ordinary exact over zero empty unicode long native-boundary nul-text nul-hash missing directory cancel alias hash-exact hash-over)
    foreach(_case IN LISTS _native_cases)
        add_test(NAME "framework.codeguard_bounded_native.${_case}"
            COMMAND umicom-codeguard-bounded-source_native-tests "${_case}" "${_root}")
        set_tests_properties("framework.codeguard_bounded_native.${_case}" PROPERTIES
            TIMEOUT 20 LABELS "codeguard;bounded-input;filesystem")
    endforeach()
    add_executable(umicom-codeguard-scan-budget-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_scan_budgets.c")
    target_link_libraries(umicom-codeguard-scan-budget-tests PRIVATE Umicom::codeguard)
    target_compile_features(umicom-codeguard-scan-budget-tests PRIVATE c_std_23)
    umicom_apply_warnings(umicom-codeguard-scan-budget-tests)
    umicom_apply_sanitizers(umicom-codeguard-scan-budget-tests)
    set(_budget_cases all-passes grow-scan grow-measure grow-header grow-studio grow-hash zero-empty zero-nonempty disabled-language no-architecture no-duplicates binary-text cancel-before direct-architecture direct-duplicate)
    foreach(_case IN LISTS _budget_cases)
        add_test(NAME "framework.codeguard_scan_budget.${_case}"
            COMMAND umicom-codeguard-scan-budget-tests "${_case}" "${_root}")
        set_tests_properties("framework.codeguard_scan_budget.${_case}" PROPERTIES
            TIMEOUT 20 LABELS "codeguard;bounded-input;scan-policy")
    endforeach()
endif()
