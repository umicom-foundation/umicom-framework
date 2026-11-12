#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/image_inspection/CMakeLists.txt
#
# PURPOSE:
#   Register memory-backed ISO inspection cases without device access.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------


add_executable(umicom-iso-image-test test_iso.c)
target_link_libraries(umicom-iso-image-test PRIVATE Umicom::iso_image)
set_target_properties(umicom-iso-image-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-iso-image-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-iso-image-test)
endif()
set(_iso_cases
    bios
    efi
    hybrid
    efi_zero
    efi_one
    efi_section_zero
    efi_section_one
    efi_section_only
    plain_iso
    non_iso
    short
    descriptor_boundary
    bios_zero
    load_range
    load_count
    efi_rest_range
    endian
    volume_zero
    volume_range
    block_size
    duplicate_primary
    duplicate_boot
    missing_primary
    missing_terminator
    descriptor_limit
    version
    catalogue_zero
    catalogue_range
    checksum
    validation_signature
    section_count
    section_empty
    section_continuation
    trailer
    hidden_trailer
    no_boot_entry
    emulation
    platform
    boot_indicator
    read_failure
    catalogue_read_failure
    prefix_absent
    prefix_incomplete
    prefix_complete
    prefix_version
    prefix_sector_minus_one
    prefix_full_sector
    prefix_next_partial
    prefix_two_minus_one
    prefix_non_iso
    prefix_read_failure
    prefix_full_non_iso
    arguments)
foreach(_case IN LISTS _iso_cases)
    add_test(NAME framework.image_inspection.iso.${_case}
        COMMAND umicom-iso-image-test "${_case}")
    set_tests_properties(framework.image_inspection.iso.${_case} PROPERTIES
        TIMEOUT 10 LABELS "framework;media;iso;no-devices")
endforeach()
