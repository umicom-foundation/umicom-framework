# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# These cases distinguish buffer checks, real process transport and explicitly
# inert pipeline fixtures. None of the fixture cases claim an operating-system boot.
add_executable(umicom-os-image-buffer-tests test_buffers.c)
target_include_directories(umicom-os-image-buffer-tests PRIVATE ../../src/os_image)
target_link_libraries(umicom-os-image-buffer-tests PRIVATE Umicom::os_image)
set_target_properties(umicom-os-image-buffer-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
set(_buffer_cases
    archive_roundtrip archive_empty archive_duplicate archive_missing_parent
    archive_file_parent archive_setid archive_block_device archive_unknown_character_device
    archive_bad_device_fields archive_entry_limit archive_file_limit archive_multiblock
    archive_crc archive_size_trailer archive_deflate_type archive_complement archive_options
    archive_extra_member archive_truncated archive_deterministic archive_mutations
    newc_roundtrip newc_uid newc_links newc_inode newc_padding newc_name_nul
    newc_trailer_missing newc_duplicate
    elf_x86 elf_riscv elf_wrong_arch elf_truncated elf_dynamic elf_interpreter elf_stack
    elf_entry elf_ph_overflow elf_memory_overflow elf_file_overflow elf_memory_small
    elf_alignment elf_big_endian elf_pie
    kernel_x86 kernel_riscv kernel_x86_flags kernel_x86_protocol kernel_x86_setup
    kernel_riscv_magic kernel_riscv_size kernel_riscv_endian
    boot_normal boot_recovery boot_crlf boot_exit boot_wrong_source boot_missing_end
    boot_service_failure boot_missing_shutdown boot_double_init boot_duplicate_field
    boot_unknown_field boot_panic boot_partial_line boot_service_order boot_service_after_report
    boot_report_state boot_incomplete_services boot_marker_substring boot_recovery_mismatch
    boot_double_report boot_embedded_nul
    manifest_roundtrip manifest_duplicate manifest_escape manifest_unknown_owner
    manifest_leading_zero manifest_version manifest_unknown_field manifest_adversarial
    artifact_matching artifact_changed artifact_wrong_path artifact_duplicate names crc32 invalid_arguments)
foreach(_case IN LISTS _buffer_cases)
    add_test(NAME framework.os_image.${_case} COMMAND umicom-os-image-buffer-tests ${_case})
    set_tests_properties(framework.os_image.${_case} PROPERTIES TIMEOUT 30 LABELS "os_image;buffer;fixture")
endforeach()
add_test(NAME framework.os_image.cli_help COMMAND umicom-os-image --help)
add_test(NAME framework.os_image.cli_self_test COMMAND umicom-os-image --self-test)
add_test(NAME framework.os_image.archive_lesson COMMAND umicom-os-image-archive-example)
set(_native_targets umicom-os-image-buffer-tests)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_program(_umi_image_git NAMES git)
    find_program(_umi_image_gzip NAMES gzip)
    find_program(_umi_image_cpio NAMES cpio)
    foreach(_tool IN ITEMS git gzip cpio)
        if(NOT _umi_image_${_tool})
            set(_umi_image_${_tool} "")
        endif()
    endforeach()
    add_executable(umicom-os-image-inert-child inert_child.c)
    add_executable(umicom-os-image-file-tests test_files.c)
    foreach(_target IN ITEMS umicom-os-image-inert-child umicom-os-image-file-tests)
        target_include_directories(${_target} PRIVATE ../../src/os_image)
        target_link_libraries(${_target} PRIVATE Umicom::os_image Threads::Threads)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        list(APPEND _native_targets ${_target})
    endforeach()
    set(_file_cases
        io_exclusive io_symlink io_symlink_parent io_fifo io_size_limit io_lock
        archive_external_tools process_arguments process_timeout process_cancel
        process_precancel process_failure process_output_cap
        pipeline_dirty_buildroot pipeline_overlap pipeline_existing_prepare
        pipeline_changed_input pipeline_lock pipeline_pack_before_build
        pipeline_changed_config pipeline_repeated_build pipeline_changed_artifact
        pipeline_changed_kernel_archive pipeline_configuration_duplicate
        pipeline_extra_bundle_file pipeline_changed_archive pipeline_existing_pack
        pipeline_legal_info pipeline_missing_qemu pipeline_boot_precancel
        pipeline_inert_normal_transport pipeline_inert_recovery_transport pipeline_riscv)
    foreach(_case IN LISTS _file_cases)
        add_test(NAME framework.os_image.${_case}
            COMMAND umicom-os-image-file-tests ${_case} $<TARGET_FILE:umicom-os-image-inert-child>
                "${_umi_image_git}" "${_umi_image_gzip}" "${_umi_image_cpio}")
        set_tests_properties(framework.os_image.${_case} PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 LABELS "os_image;linux;inert_transport")
    endforeach()
endif()
foreach(_target IN LISTS _native_targets)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${_target})
    endif()
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${_target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${_target})
    endif()
endforeach()

if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-os-image)
    umicom_register_validation_target(umicom-os-image-archive-example)
endif()
