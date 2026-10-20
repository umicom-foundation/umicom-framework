# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
get_filename_component(_kanban_storage_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_executable(umi-kanban-document-test "${_kanban_storage_root}/tests/kanban_storage/test_document.c")
target_link_libraries(umi-kanban-document-test PRIVATE Umicom::kanban_storage)
set_target_properties(umi-kanban-document-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umi-kanban-document-test)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umi-kanban-document-test PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umi-kanban-document-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-document-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-document-test FILES
        "${_kanban_storage_root}/tests/kanban/fixture.h"
        "${_kanban_storage_root}/tests/build_log/fixture.h"
        "${_kanban_storage_root}/tests/support/symbolic_link.h")
endif()
foreach(_case IN ITEMS roundtrip unicode working-directory existing ownership stale
        external-change external-delete discard copy write-refusal invalid-path
        missing-parent malformed symlink-board symlink-guard)
    set(_test "framework.kanban.storage.${_case}")
    # Only these named cases write here. Fresh exclusive subdirectories preserve
    # failure evidence and allow repeated or parallel owner-run qualification.
    set(_work "${CMAKE_CURRENT_BINARY_DIR}/kanban-storage-fixtures/${_case}")
    file(MAKE_DIRECTORY "${_work}")
    add_test(NAME "${_test}" COMMAND umi-kanban-document-test "${_case}")
    set_tests_properties("${_test}" PROPERTIES WORKING_DIRECTORY "${_work}"
        TIMEOUT 30 LABELS "framework;kanban;storage;regression")
    if(_case MATCHES "^symlink-")
        set_tests_properties("${_test}" PROPERTIES SKIP_RETURN_CODE 77)
    endif()
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umi-kanban-document-test)
    endif()
endforeach()

# Keep memory-limit exercises separate from ordinary save/reopen fixtures.
add_executable(umi-kanban-storage-limits-test "${_kanban_storage_root}/tests/kanban_storage/test_boundaries.c")
target_link_libraries(umi-kanban-storage-limits-test PRIVATE Umicom::kanban_storage)
set_target_properties(umi-kanban-storage-limits-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umi-kanban-storage-limits-test)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umi-kanban-storage-limits-test PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umi-kanban-storage-limits-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-storage-limits-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-storage-limits-test FILES
        "${_kanban_storage_root}/tests/kanban/fixture.h"
        "${_kanban_storage_root}/tests/build_log/fixture.h"
        "${_kanban_storage_root}/tests/support/symbolic_link.h")
endif()
foreach(_case IN ITEMS oversized-open oversized-save clean-change guard-contents)
    set(_test "framework.kanban.storage.${_case}")
    set(_work "${CMAKE_CURRENT_BINARY_DIR}/kanban-storage-fixtures/${_case}")
    file(MAKE_DIRECTORY "${_work}")
    add_test(NAME "${_test}" COMMAND umi-kanban-storage-limits-test "${_case}")
    set_tests_properties("${_test}" PROPERTIES WORKING_DIRECTORY "${_work}"
        TIMEOUT 30 LABELS "framework;kanban;storage;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umi-kanban-storage-limits-test)
    endif()
endforeach()
