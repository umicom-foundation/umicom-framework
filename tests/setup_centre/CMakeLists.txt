# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-setup-centre-test test_setup.c test_install.c test_vm.c)
target_include_directories(umicom-setup-centre-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/setup_centre")
target_link_libraries(umicom-setup-centre-test PRIVATE Umicom::setup_centre)
umicom_apply_warnings(umicom-setup-centre-test)
umicom_apply_sanitizers(umicom-setup-centre-test)
set(_cases
    paths.valid paths.unsafe paths.unicode paths.absolute paths.capacity numbers.overflow
    catalogue.roundtrip catalogue.duplicate catalogue.unknown catalogue.missing_entry
    catalogue.reserved catalogue.nonadjacent_parent catalogue.sixty_four_apps
    catalogue.overflow catalogue.case_collision catalogue.parent_collision catalogue.mutations
    pack.bad_source pack.cancel pack.no_script_evaluation pack.blank_line pack.deduplicate pack.destination_exists
    review.no_writes review.empty_selection review.unknown_selection review.cancel
    review.changed_catalogue review.changed_payload review.unselected_missing
    install.plan_mismatch install.destination_changed install.exists install.cancel_copy
    install.cancel_before install.symlink install.parent_symlink install.all install.subset install.repeat
    verify.tamper verify.missing verify.user_file
    vm.limits vm.bad_kernel vm.no_script vm.comma vm.plan vm.riscv vm.interactive
    vm.stale vm.wrong_review vm.failed_exit vm.cancelled vm.execute_contract
    media.prepare media.exists media.bad_arch media.no_script media.tampered media.iso_structure
    media.producer_failure media.missing_iso)
foreach(_case IN LISTS _cases)
    add_test(NAME framework.setup_centre.${_case} COMMAND umicom-setup-centre-test "${_case}")
    set_tests_properties(framework.setup_centre.${_case} PROPERTIES TIMEOUT 90 SKIP_RETURN_CODE 77
        LABELS "framework;setup;native")
endforeach()
add_test(NAME framework.setup_centre.cli_help COMMAND umicom-setup --help)
add_test(NAME framework.setup_centre.cli_self_test COMMAND umicom-setup --self-test)

# These are real process-adapter tests. Register only when the canonical
# process target is present; do not substitute a fake platform for this check.
if(TARGET Umicom::setup_process)
    add_executable(umicom-setup-process-child process_child.c)
    add_executable(umicom-setup-process-test test_process.c)
    target_link_libraries(umicom-setup-process-test PRIVATE Umicom::setup_process)
    foreach(_target IN ITEMS umicom-setup-process-child umicom-setup-process-test)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
    endforeach()
    foreach(_case IN ITEMS arguments timeout pre_cancelled exit_failure missing_program bad_directory)
        add_test(NAME framework.setup_centre.process.${_case}
            COMMAND umicom-setup-process-test "${_case}" "$<TARGET_FILE:umicom-setup-process-child>")
        set_tests_properties(framework.setup_centre.process.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;setup;native;process")
    endforeach()
endif()

if(TARGET umicom-setup-centre)
    add_test(NAME framework.setup_centre.windows_ui COMMAND umicom-setup-centre --check-ui)
    set_tests_properties(framework.setup_centre.windows_ui PROPERTIES
        TIMEOUT 30 LABELS "framework;setup;windows;gui")
endif()
