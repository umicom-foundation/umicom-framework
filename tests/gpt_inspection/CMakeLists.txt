#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/gpt_inspection/CMakeLists.txt
#
# PURPOSE:
#   Register memory-only GPT structure and allocation-refusal qualification.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

add_executable(umicom-gpt-image-test test_gpt.c)
target_link_libraries(umicom-gpt-image-test PRIVATE Umicom::gpt_image)
add_executable(umicom-gpt-allocation-test test_allocation.c)
target_include_directories(umicom-gpt-allocation-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../include")
foreach(_gpt_target IN ITEMS umicom-gpt-image-test umicom-gpt-allocation-test)
    set_target_properties(${_gpt_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${_gpt_target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${_gpt_target})
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${_gpt_target})
    endif()
endforeach()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-gpt-allocation-test
        FILES "${CMAKE_CURRENT_LIST_DIR}/../../src/image_inspection/gpt.c")
endif()
foreach(_gpt_case IN ITEMS
        valid
        four-k
        small-slot-count
        wide-entries
        widest-entries
        largest-profile
        empty
        unused-fields
        adjacent
        one-sector
        attributes
        null-reader
        null-output
        zero-bytes
        short
        unaligned
        bad-block
        signature
        revision
        header-small
        header-large
        header-crc
        header-reserved
        header-padding
        header-location
        alternate
        disk-guid
        count-zero
        count-large
        stride-small
        stride-nonpower
        stride-large
        primary-reservation
        backup-reservation
        primary-table
        backup-table
        first-last
        last-backup
        backup-signature
        backup-crc
        backup-location
        backup-alternate
        backup-guid
        backup-first
        backup-last
        backup-count
        backup-width
        backup-table-crc
        backup-size
        table-crc
        table-mismatch
        missing-partition-guid
        partition-before
        partition-after
        partition-inverted
        partition-overlap
        duplicate-guid
        entry-reserved
        unused-reserved
        reserved-attributes
        read-primary
        read-backup
        read-table
        read-backup-table)
    add_test(NAME "framework.image_inspection.gpt.${_gpt_case}"
        COMMAND umicom-gpt-image-test "${_gpt_case}")
    set_tests_properties("framework.image_inspection.gpt.${_gpt_case}" PROPERTIES
        TIMEOUT 10 LABELS "framework;media;gpt;no-devices")
endforeach()
foreach(_gpt_refusal IN ITEMS first second)
    add_test(NAME "framework.image_inspection.gpt.allocation-${_gpt_refusal}"
        COMMAND umicom-gpt-allocation-test "${_gpt_refusal}")
    set_tests_properties("framework.image_inspection.gpt.allocation-${_gpt_refusal}" PROPERTIES
        TIMEOUT 10 LABELS "framework;media;gpt;no-devices")
endforeach()
unset(_gpt_target)
unset(_gpt_case)
unset(_gpt_refusal)
