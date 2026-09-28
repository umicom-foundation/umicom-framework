# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
if(NOT TARGET umicom-ai-evidence)
    message(FATAL_ERROR "AI evidence tool is required for the lesson test")
endif()
add_test(NAME framework.ai_evidence.lesson COMMAND umicom-ai-evidence --self-test)
set_tests_properties(framework.ai_evidence.lesson PROPERTIES TIMEOUT 45)
add_executable(umicom-ai-evidence-test "${CMAKE_CURRENT_LIST_DIR}/test_evidence.c")
target_link_libraries(umicom-ai-evidence-test PRIVATE Umicom::ai_workspace)
umicom_ai_workspace_configure_target(umicom-ai-evidence-test)
set(_umi_ai_evidence_cases
    selection_order selection_duplicates selection_zero selection_capacity
    selection_nan selection_infinite selection_negative selection_zero_score
    selection_forged_text selection_forged_title selection_forged_lines selection_forged_revision
    selection_wrong_collection selection_unterminated selection_stale selection_removed
    duplicate_identity hybrid_and_default
    capture_pending capture_succeeded capture_changed capture_removed capture_unrelated capture_snapshot
    lines_plain lines_unicode lines_high_number
    reference_grammar reference_bounds reference_mutations atomic_request format sqlite_reload review_gate)
foreach(case IN LISTS _umi_ai_evidence_cases)
    add_test(NAME "framework.ai_evidence.${case}" COMMAND umicom-ai-evidence-test "${case}"
        "${CMAKE_CURRENT_BINARY_DIR}/ai-evidence-${case}.sqlite")
    set_tests_properties("framework.ai_evidence.${case}" PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
        LABELS "framework;ai-evidence;regression")
endforeach()
unset(_umi_ai_evidence_cases)
# Native fixture is primary for this new end-to-end path. Existing Python
# protocol fixtures are retained independently; no product depends on Python.
if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(Threads REQUIRED)
    add_executable(umicom-ai-evidence-http-test "${CMAKE_CURRENT_LIST_DIR}/test_native_http.c")
    target_link_libraries(umicom-ai-evidence-http-test PRIVATE Umicom::ai_workspace Threads::Threads)
    if(WIN32)
        target_link_libraries(umicom-ai-evidence-http-test PRIVATE ws2_32)
    endif()
    umicom_ai_workspace_configure_target(umicom-ai-evidence-http-test)
    foreach(case success unicode model_mismatch tool_call bad_citation bad_json busy redirect disconnect timeout cancel)
        add_test(NAME "framework.ai_evidence.native_http.${case}" COMMAND umicom-ai-evidence-http-test "${case}")
        set_tests_properties("framework.ai_evidence.native_http.${case}" PROPERTIES TIMEOUT 15 SKIP_RETURN_CODE 77
            LABELS "framework;ai-evidence;native-http;loopback")
    endforeach()
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-ai-evidence-allocation-test "${CMAKE_CURRENT_LIST_DIR}/test_allocations.c")
    target_link_libraries(umicom-ai-evidence-allocation-test PRIVATE Umicom::ai_workspace)
    target_link_options(umicom-ai-evidence-allocation-test PRIVATE -Wl,--wrap=malloc -Wl,--wrap=calloc)
    umicom_ai_workspace_configure_target(umicom-ai-evidence-allocation-test)
    add_test(NAME framework.ai_evidence.allocation_failures COMMAND umicom-ai-evidence-allocation-test)
    set_tests_properties(framework.ai_evidence.allocation_failures PROPERTIES TIMEOUT 45 LABELS "framework;ai-evidence;fault-injection")
endif()
if(TARGET Umicom::ai_workspace_gtk4)
    add_executable(umicom-ai-evidence-gtk-test "${CMAKE_CURRENT_LIST_DIR}/test_gtk.c")
    target_link_libraries(umicom-ai-evidence-gtk-test PRIVATE Umicom::ai_workspace_gtk4)
    umicom_ai_workspace_configure_target(umicom-ai-evidence-gtk-test)
    foreach(case pending succeeded changed selected_preparation stale_selection retained_button)
        add_test(NAME "framework.ai_evidence.gtk.${case}" COMMAND umicom-ai-evidence-gtk-test "${case}")
        set_tests_properties("framework.ai_evidence.gtk.${case}" PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            LABELS "framework;ai-evidence;gtk4;lifecycle")
    endforeach()
endif()
