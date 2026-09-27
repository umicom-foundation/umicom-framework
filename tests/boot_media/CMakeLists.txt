# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-boot-media-test test_media.c)
target_include_directories(umicom-boot-media-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media")
target_link_libraries(umicom-boot-media-test PRIVATE Umicom::boot_media Umicom::platform)
set_target_properties(umicom-boot-media-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-boot-media-test)
umicom_apply_sanitizers(umicom-boot-media-test)
set(_cases
    crc.vector path.capacity path.drive_root device.write_gate
    mbr.valid mbr.range mbr.flag mbr.signature mbr.no_loader mbr.zero_partition mbr.empty_type mbr.overlap
    gpt.valid gpt.header_crc gpt.table_crc gpt.backup gpt.backup_table gpt.large_count
    iso.valid iso.bios iso.endian iso.terminator iso.catalogue iso.load_range iso.section_count iso.version iso.zero_load
    image.short image.non_sector
    copy.complete copy.exists copy.same copy.bad_review copy.bad_confirmation copy.source_changed copy.collision
    copy.cancel_before copy.cancel_mid copy.cancel_verify copy.cancel_final copy.repeat copy.verify_tamper
    policy.valid policy.system policy.mounted policy.fixed policy.not_usb policy.readonly policy.unknown
    policy.small policy.large policy.sector policy.4kn policy.optical_only policy.gpt_resize policy.unterminated_model policy.unterminated_serial policy.unterminated_identity)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND _cases source.symlink source.hardlink source.fifo copy.source_during copy.target_during image.mutations image.changed_during_inspection)
endif()
foreach(_case IN LISTS _cases)
    add_test(NAME framework.boot_media.${_case} COMMAND umicom-boot-media-test "${_case}")
    set_tests_properties(framework.boot_media.${_case} PROPERTIES TIMEOUT 90 LABELS "framework;media;native;no-devices")
endforeach()
add_test(NAME framework.boot_media.cli_self_test COMMAND umicom-media --self-test)
add_test(NAME framework.boot_media.cli_help COMMAND umicom-media --help)
add_test(NAME framework.boot_media.image_example_memory COMMAND umicom-media-image-example)
add_test(NAME framework.boot_media.policy_example COMMAND umicom-media-review-example)
if(WIN32)
    add_executable(umicom-windows-media-sdk-test test_windows_sdk.c)
    target_link_libraries(umicom-windows-media-sdk-test PRIVATE iphlpapi ws2_32)
    set_target_properties(umicom-windows-media-sdk-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-windows-media-sdk-test)
    add_test(NAME framework.boot_media.windows_sdk COMMAND umicom-windows-media-sdk-test)
    if(TARGET umicom-media-centre)
        add_test(NAME framework.boot_media.windows_ui COMMAND umicom-media-centre --check-ui)
        set_tests_properties(framework.boot_media.windows_ui PROPERTIES TIMEOUT 30)
    endif()
endif()

add_test(NAME framework.boot_media.example_file_workflow COMMAND umicom-boot-media-test
    example.file_workflow "$<TARGET_FILE:umicom-media-image-example>")
set_tests_properties(framework.boot_media.example_file_workflow PROPERTIES TIMEOUT 30 LABELS "framework;media;native;no-devices")
