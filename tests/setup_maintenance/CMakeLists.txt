# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-setup-maintenance-test test_maintenance.c
    "${CMAKE_CURRENT_LIST_DIR}/../../examples/setup_maintenance/review_client.c")
target_link_libraries(umicom-setup-maintenance-test PRIVATE Umicom::setup_centre)
target_include_directories(umicom-setup-maintenance-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/setup_centre")
set_target_properties(umicom-setup-maintenance-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-setup-maintenance-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-setup-maintenance-test)
endif()
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/cases")
set(_cases update undo-update repair undo-repair remove-one remove-edited remove-all undo-remove no-change edited-update unowned-collision repair-wrong-release remove-retained-damage bad-mask damaged-release wrong-fingerprint source-changed receipt-changed installed-changed double-apply journal-owner malformed-pending cancel-0 cancel-1 cancel-2 cancel-3 cancel-4 recovery-interrupted repeat-after-recovery journal-tamper before-tamper live-recovery-edit wrong-transaction journal-roundtrip journal-truncation journal-mutations journal-nul)
list(APPEND _cases cli-pending-none cli-pending-present public-example bound-list stale-list owned-config missing-app changed-entry reserved-control recover-completed undo-local-edit undo-newer undo-selective repair-title-change cancel-before-pending undo-interrupted missing-backup identity-tamper null-contracts staged-tamper destination-race)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND _cases symlink-file hardlink-file fifo-file directory-file writable-root move-no-replace exclusive-lock process-crash recovery-process-crash)
endif()
foreach(_case IN LISTS _cases)
    add_test(NAME framework.setup_maintenance.${_case}
        COMMAND umicom-setup-maintenance-test "${_case}" "${CMAKE_CURRENT_BINARY_DIR}/cases")
    set_tests_properties(framework.setup_maintenance.${_case} PROPERTIES TIMEOUT 60 LABELS "setup;maintenance;native")
endforeach()
add_test(NAME framework.setup_maintenance.selftest COMMAND umicom-maintain --self-test)
add_test(NAME framework.setup_maintenance.help COMMAND umicom-maintain --help)
# Reuse Batch 23's real Clang/LLD images for a full native file-maintenance
# integration. These are PE-format inputs, not runnable Windows qualification.
if(TARGET umicom-release-linked-pe-fixtures AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    get_target_property(_umi_inspector_binary umicom-release-inspector-test BINARY_DIR)
    add_executable(umicom-maintenance-linked-release-test test_linked_release.c)
    target_include_directories(umicom-maintenance-linked-release-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/setup_centre")
    target_link_libraries(umicom-maintenance-linked-release-test PRIVATE Umicom::setup_centre Umicom::release_inspector)
    set_target_properties(umicom-maintenance-linked-release-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-maintenance-linked-release-test)
    umicom_apply_sanitizers(umicom-maintenance-linked-release-test)
    add_dependencies(umicom-maintenance-linked-release-test umicom-release-linked-pe-fixtures)
    add_test(NAME framework.setup_maintenance.linked_release
        COMMAND umicom-maintenance-linked-release-test
            "${_umi_inspector_binary}/linked-pe/consumer.exe"
            "${_umi_inspector_binary}/linked-pe/delayed.exe"
            "${_umi_inspector_binary}/linked-pe/umicom-practice-storage.dll"
            "${_umi_inspector_binary}/linked-pe/bootstrap.exe")
    set_tests_properties(framework.setup_maintenance.linked_release PROPERTIES TIMEOUT 60
        LABELS "setup;maintenance;native;format-only;not-windows-runtime")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-setup-maintenance-test)
    if(TARGET umicom-maintenance-linked-release-test)
        umicom_register_validation_target(umicom-maintenance-linked-release-test)
    endif()
endif()
