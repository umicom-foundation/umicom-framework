# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Persistence is an optional link dependency, separate from basic CTest execution.
include_guard(GLOBAL)
add_library(umicom_test_archive STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/archive.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/archive_codec.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/archive_records.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/archive_write.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/archive_review.c")
add_library(Umicom::test_archive ALIAS umicom_test_archive)
set_target_properties(umicom_test_archive PROPERTIES EXPORT_NAME test_archive
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_test_archive PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_test_archive PUBLIC Umicom::testing Umicom::data Umicom::platform)
umicom_apply_warnings(umicom_test_archive)
umicom_apply_sanitizers(umicom_test_archive)
target_link_libraries(umicom_framework INTERFACE Umicom::test_archive)
install(TARGETS umicom_test_archive EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/private-test-archives.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    set(codec_cases entry request result exit-range bounded unterminated bad-count false-pass
        format truncated trailing overflow negative nul invalid-enum)
    set(store_cases invalid active outer-transaction cancel-before cancel-during identity summary capacity
        exhausted-id roundtrip privacy large-output never-started missing namespace remove missing-part
        short-part chunk-overflow missing-metadata duplicate corrupt memory-full scope-boundary maximal-fields)
    set(sqlite_cases reopen write-failure commit-failure busy remove-failure)
    set(worker_cases active created success cancel lifetime storage-failure late-stop)
    set(capture_cases selection disabled cancelled)
    set(compare_cases regression mixed persisting timeout report-error recovered skipped cancelled not-run
        never-started partial root repeat stop-policy timeout-policy enabled build-root configuration renamed
        duplicate reordered duration revision invalid missing cancel transaction corrupt-summary missing-chunk
        detached self)
    set(review_cases created missing cancel success late-stop lifetime)
    foreach(kind codec store sqlite worker capture compare review)
        set(target "umicom-test-archive-${kind}-test")
        add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/test_archive/test_${kind}.c")
        target_link_libraries(${target} PRIVATE Umicom::test_archive)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
        foreach(case IN LISTS ${kind}_cases)
            add_test(NAME framework.test_archive.${kind}.${case} COMMAND ${target} "${case}")
            set_tests_properties(framework.test_archive.${kind}.${case} PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;testing;archive;persistence;regression")
        endforeach()
    endforeach()
endif()
