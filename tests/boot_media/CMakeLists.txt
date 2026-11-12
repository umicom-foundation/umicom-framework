#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/boot_media/CMakeLists.txt
#
# PURPOSE:
#   Register media contract and native application composition checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
add_executable(umicom-boot-media-test test_media.c)
target_include_directories(umicom-boot-media-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media")
target_link_libraries(umicom-boot-media-test PRIVATE Umicom::boot_media Umicom::platform)
set_target_properties(umicom-boot-media-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-boot-media-test)
umicom_apply_sanitizers(umicom-boot-media-test)
set(_cases
    extent.zero extent.valid extent.wrong_expected extent.null extent.large_expected extent.grown extent.shrunk extent.readonly
    cli.arguments crc.vector path.capacity path.drive_root device.write_gate
    mbr.valid mbr.range mbr.flag mbr.signature mbr.no_loader mbr.zero_partition mbr.empty_type mbr.overlap
    gpt.valid gpt.header_crc gpt.table_crc gpt.backup gpt.backup_table gpt.large_count
    gpt.stride gpt.primary_reservation gpt.backup_reservation gpt.reserved_attributes
    gpt.protective_short gpt.protective_zero_start gpt.protective_past_end gpt.protective_shifted gpt.protective_duplicate gpt.protective_missing
    iso.valid iso.bios iso.endian iso.terminator iso.catalogue iso.load_range iso.section_count iso.version iso.zero_load iso.efi_zero iso.efi_one iso.hidden_trailer
    image.short image.non_sector
    copy.complete copy.exists copy.same copy.bad_review copy.bad_confirmation copy.source_changed copy.collision
    copy.cancel_before copy.cancel_mid copy.cancel_verify copy.cancel_final copy.repeat copy.verify_tamper
    policy.valid policy.system policy.mounted policy.fixed policy.not_usb policy.readonly policy.unknown
    policy.small policy.large policy.sector policy.4kn policy.optical_only policy.gpt_resize policy.unterminated_model policy.unterminated_serial policy.unterminated_identity)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND _cases
        image.extent_grown_inspection copy.extent_grown_recheck copy.extent_grown_before_acquire
        copy.extent_grown_write copy.extent_grown_verify copy.extent_source_after_flush
        verify.extent_grown verify.extent_source_grown)
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
        add_test(NAME framework.boot_media.windows_selection COMMAND umicom-media-centre --check-selection)
        add_test(NAME framework.boot_media.windows_labels COMMAND umicom-media-centre --check-labels)
        add_test(NAME framework.boot_media.windows_reports COMMAND umicom-media-centre --check-reports)
        set_tests_properties(framework.boot_media.windows_reports PROPERTIES
            TIMEOUT 30 LABELS "framework;media;native;windows;no-devices")
        set_tests_properties(framework.boot_media.windows_selection framework.boot_media.windows_labels
            PROPERTIES TIMEOUT 30 LABELS "framework;media;native;windows;no-devices")
    endif()
endif()

add_test(NAME framework.boot_media.example_file_workflow COMMAND umicom-boot-media-test
    example.file_workflow "$<TARGET_FILE:umicom-media-image-example>")
set_tests_properties(framework.boot_media.example_file_workflow PROPERTIES TIMEOUT 30 LABELS "framework;media;native;no-devices")

if(NOT CMAKE_CROSSCOMPILING)
    add_test(NAME framework.boot_media.native_branding
        COMMAND "${CMAKE_COMMAND}"
            "-DROOT=${CMAKE_CURRENT_LIST_DIR}/../.."
            "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/native-branding"
            "-DCOMPILER=${CMAKE_C_COMPILER}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../build_configuration/check_native_branding.cmake")
    set_tests_properties(framework.boot_media.native_branding PROPERTIES TIMEOUT 180
        LABELS "framework;media;branding;configuration;no-devices")
endif()

# Pure policy observations never discover, acquire or write a physical device.
add_executable(umicom-media-policy-observation-test test_policy_observation.c)
target_link_libraries(umicom-media-policy-observation-test PRIVATE Umicom::boot_media)
set_target_properties(umicom-media-policy-observation-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-media-policy-observation-test)
umicom_apply_sanitizers(umicom-media-policy-observation-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-media-policy-observation-test)
endif()
foreach(_case IN ITEMS valid
        null-image
        null-device
        null-report
        repeat
        unicode
        empty-reason
        system
        mounted
        fixed
        not-usb
        readonly
        small
        large
        sector
        4kn
        optical
        gpt-exact
        gpt-resize
        flag-mbr-negative
        flag-mbr-large
        flag-gpt-negative
        flag-gpt-large
        flag-iso-negative
        flag-iso-large
        flag-el-torito-negative
        flag-el-torito-large
        flag-bios-negative
        flag-bios-large
        flag-efi-negative
        flag-efi-large
        flag-usb-negative
        flag-usb-large
        flag-removable-negative
        flag-removable-large
        flag-protected-negative
        flag-protected-large
        flag-in-use-negative
        flag-in-use-large
        flag-read-only-negative
        flag-read-only-large
        text-path-unterminated
        text-path-invalid
        text-identity-unterminated
        text-identity-invalid
        text-model-unterminated
        text-model-invalid
        text-serial-unterminated
        text-serial-invalid
        text-reason-unterminated
        text-reason-invalid
        empty-path
        empty-identity
        empty-model
        empty-serial)
    add_test(NAME "framework.boot_media.policy_observation.${_case}"
        COMMAND umicom-media-policy-observation-test "${_case}")
    set_tests_properties("framework.boot_media.policy_observation.${_case}" PROPERTIES
        TIMEOUT 15 LABELS "framework;media;observation;no-devices;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("framework.boot_media.policy_observation.${_case}"
            TARGETS umicom-media-policy-observation-test)
    endif()
endforeach()

# Compile the real diagnostic boundary with a narrow synthetic observation or
# allocation seam. These scenarios never discover or acquire physical media.
add_executable(umicom-media-discovery-report-test test_discovery_report.c)
target_link_libraries(umicom-media-discovery-report-test PRIVATE Umicom::boot_media)
set_target_properties(umicom-media-discovery-report-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-media-discovery-report-test)
umicom_apply_sanitizers(umicom-media-discovery-report-test)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-media-discovery-report-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media/discovery.c")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-media-discovery-report-test)
endif()
foreach(_case IN ITEMS success unavailable capacity permission null-output null-report empty-success empty-failure adapter-stale repeat)
    add_test(NAME framework.boot_media.discovery_report.${_case}
        COMMAND umicom-media-discovery-report-test ${_case})
    set_tests_properties(framework.boot_media.discovery_report.${_case} PROPERTIES
        TIMEOUT 15 LABELS "framework;media;diagnostic;no-devices;regression")
    if(COMMAND umicom_test_impact_owners)
        # File dependency metadata belongs to the target qualification service.
        # Keep the former unsupported argument form disabled for source review.
        if(FALSE)
        umicom_test_impact_owners(framework.boot_media.discovery_report.${_case}
            TARGETS umicom-media-discovery-report-test
            INPUTS "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media/discovery.c")
        endif()
        umicom_test_impact_owners(framework.boot_media.discovery_report.${_case}
            TARGETS umicom-media-discovery-report-test)
    endif()
endforeach()

# Compile the real diagnostic boundary with a narrow synthetic observation or
# allocation seam. These scenarios never discover or acquire physical media.
add_executable(umicom-media-refusal-report-test test_refusal_report.c)
target_link_libraries(umicom-media-refusal-report-test PRIVATE Umicom::boot_media)
set_target_properties(umicom-media-refusal-report-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-media-refusal-report-test)
umicom_apply_sanitizers(umicom-media-refusal-report-test)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-media-refusal-report-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media/transfer.c")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-media-refusal-report-test)
endif()
foreach(_case IN ITEMS inspect-no-output inspect-null-path inspect-relative review-null-output review-null-source review-relative-source review-null-target review-relative-target review-long-target review-no-memory device-null device-path device-identity device-model device-serial device-reason device-null-output apply-null-plan apply-null-fingerprint apply-null-confirmation apply-mismatch apply-consumed verify-null-source verify-relative-source discover-null-output)
    add_test(NAME framework.boot_media.refusal_report.${_case}
        COMMAND umicom-media-refusal-report-test ${_case})
    set_tests_properties(framework.boot_media.refusal_report.${_case} PROPERTIES
        TIMEOUT 15 LABELS "framework;media;diagnostic;no-devices;regression")
    if(COMMAND umicom_test_impact_owners)
        # File dependency metadata belongs to the target qualification service.
        # Keep the former unsupported argument form disabled for source review.
        if(FALSE)
        umicom_test_impact_owners(framework.boot_media.refusal_report.${_case}
            TARGETS umicom-media-refusal-report-test
            INPUTS "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media/transfer.c")
        endif()
        umicom_test_impact_owners(framework.boot_media.refusal_report.${_case}
            TARGETS umicom-media-refusal-report-test)
    endif()
endforeach()

# Creation effects survive post-open refusal; no test acquires a physical device.
add_executable(umicom-media-creation-effects-test test_creation_effects.c)
target_link_libraries(umicom-media-creation-effects-test PRIVATE Umicom::boot_media Umicom::platform)
set_target_properties(umicom-media-creation-effects-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-media-creation-effects-test)
umicom_apply_sanitizers(umicom-media-creation-effects-test)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-media-creation-effects-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media/transfer.c")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-media-creation-effects-test)
endif()
# Preserve the former case list while extending checked-release coverage.
if(FALSE)
foreach(_case IN ITEMS complete open-refused existing created-no-memory created-invalid created-lock-refused buffer-no-memory cancel-before-acquire cancel-before-payload write-error flush-error verify-error finished-cancel)
endforeach()
endif()
foreach(_case IN ITEMS complete open-refused existing created-no-memory created-invalid created-lock-refused buffer-no-memory cancel-before-acquire cancel-before-payload write-error flush-error verify-error finished-cancel target-close-error source-close-error both-close-errors write-and-close-error cancelled-and-close-error)
    add_test(NAME framework.boot_media.creation_effects.${_case} COMMAND umicom-media-creation-effects-test ${_case})
    set_tests_properties(framework.boot_media.creation_effects.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;media;no-devices;regression;injected-effects")

    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.boot_media.creation_effects.${_case} TARGETS umicom-media-creation-effects-test)
    endif()
endforeach()

add_executable(umicom-media-open-observation-test test_open_observation.c)
target_link_libraries(umicom-media-open-observation-test PRIVATE Umicom::boot_media Umicom::platform)
set_target_properties(umicom-media-open-observation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-media-open-observation-test)
umicom_apply_sanitizers(umicom-media-open-observation-test)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-media-open-observation-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media/io.c")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-media-open-observation-test)
endif()
foreach(_case IN ITEMS success existing no-memory metadata-failure invalid-path missing-parent null-output null-observation unicode legacy)
    add_test(NAME framework.boot_media.open_observation.${_case} COMMAND umicom-media-open-observation-test ${_case})
    set_tests_properties(framework.boot_media.open_observation.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;media;no-devices;regression;native-fixture")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/media-open-${_case}")
    set_tests_properties(framework.boot_media.open_observation.${_case} PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/media-open-${_case}")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.boot_media.open_observation.${_case} TARGETS umicom-media-open-observation-test)
    endif()
endforeach()

# Inject transport release outcomes without opening files or devices.
add_executable(umicom-media-close-observation-test test_close_observation.c)
target_link_libraries(umicom-media-close-observation-test PRIVATE Umicom::boot_media Umicom::platform)
set_target_properties(umicom-media-close-observation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-media-close-observation-test)
umicom_apply_sanitizers(umicom-media-close-observation-test)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-media-close-observation-test FILES "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media/io.c")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-media-close-observation-test)
endif()
set(_media_close_cases null success main-failure legacy legacy-failure)
if(WIN32)
    list(APPEND _media_close_cases first-volume middle-volume last-volume multiple-failures)
else()
    list(APPEND _media_close_cases interrupted)
endif()
foreach(_case IN LISTS _media_close_cases)
    add_test(NAME framework.boot_media.close_observation.${_case} COMMAND umicom-media-close-observation-test ${_case})
    set_tests_properties(framework.boot_media.close_observation.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;media;no-devices;regression;injected-release")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.boot_media.close_observation.${_case} TARGETS umicom-media-close-observation-test)
    endif()
endforeach()

# Inject transport release outcomes without opening files or devices.
add_executable(umicom-media-read-close-test test_read_close.c)
target_link_libraries(umicom-media-read-close-test PRIVATE Umicom::boot_media Umicom::platform)
set_target_properties(umicom-media-read-close-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-media-read-close-test)
umicom_apply_sanitizers(umicom-media-read-close-test)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-media-read-close-test FILES "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media/transfer.c")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-media-read-close-test)
endif()
set(_media_read_close_cases inspect-success inspect-close-error inspect-read-and-close-error verify-success verify-source-close-error verify-target-close-error verify-read-and-close-error null-report)
foreach(_case IN LISTS _media_read_close_cases)
    add_test(NAME framework.boot_media.read_close.${_case} COMMAND umicom-media-read-close-test ${_case})
    set_tests_properties(framework.boot_media.read_close.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;media;no-devices;regression;injected-release")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.boot_media.read_close.${_case} TARGETS umicom-media-read-close-test)
    endif()
endforeach()

# Describe final observations without opening files or devices.
add_executable(umicom-media-report-description-test test_report_description.c)
target_link_libraries(umicom-media-report-description-test PRIVATE Umicom::boot_media Umicom::platform)
set_target_properties(umicom-media-report-description-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-media-report-description-test)
umicom_apply_sanitizers(umicom-media-report-description-test)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-media-report-description-test FILES "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media/report.c")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-media-report-description-test)
endif()
set(_media_report_cases empty created-failure verified-read verified-copy maximum-detail unicode maximum-progress exact-capacity short-capacity zero-capacity null-report null-output mismatched-status unknown-status negative-phase unknown-phase created-flag write-flag verified-flag verified-failure progress-overflow unterminated-detail invalid-utf8 refusal-preserves-previous)
foreach(_case IN LISTS _media_report_cases)
    add_test(NAME framework.boot_media.report_description.${_case} COMMAND umicom-media-report-description-test ${_case})
    set_tests_properties(framework.boot_media.report_description.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;media;no-devices;regression;report-contract")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.boot_media.report_description.${_case} TARGETS umicom-media-report-description-test)
    endif()
endforeach()

# Describe final observations without opening files or devices.
add_executable(umicom-media-cli-report-test test_cli_report.c)
target_link_libraries(umicom-media-cli-report-test PRIVATE Umicom::boot_media Umicom::platform)
set_target_properties(umicom-media-cli-report-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-media-cli-report-test)
umicom_apply_sanitizers(umicom-media-cli-report-test)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-media-cli-report-test FILES "${CMAKE_CURRENT_LIST_DIR}/../../src/boot_media/cli.c")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-media-cli-report-test)
endif()
set(_media_cli_report_cases success created-failure unavailable mismatched-report malformed-report device-refusal allocation-refusal)
foreach(_case IN LISTS _media_cli_report_cases)
    add_test(NAME framework.boot_media.cli_report.${_case} COMMAND umicom-media-cli-report-test ${_case})
    set_tests_properties(framework.boot_media.cli_report.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;media;no-devices;regression;report-contract")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.boot_media.cli_report.${_case} TARGETS umicom-media-cli-report-test)
    endif()
endforeach()


if(WIN32)
    # The real hidden controls and native thread are exercised with synthetic
    # media work. Neither image acquisition nor physical-device discovery runs.
    add_executable(umicom-media-worker-windows-test test_worker_windows.c)
    target_link_libraries(umicom-media-worker-windows-test PRIVATE
        Umicom::boot_media user32 gdi32 comctl32 comdlg32)
    target_compile_definitions(umicom-media-worker-windows-test PRIVATE UNICODE _UNICODE)
    set_target_properties(umicom-media-worker-windows-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-media-worker-windows-test)
    umicom_apply_sanitizers(umicom-media-worker-windows-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-media-worker-windows-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../../adapters/win32/boot_media.c")
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-media-worker-windows-test)
    endif()
    foreach(_case IN ITEMS completion lost-notification premature-notification stale-progress close-running quit-running window-destroyed wait-failure idle-loop-error drain-failure start-failure admission-close release-failure)
        add_test(NAME framework.boot_media.windows_worker.${_case}
            COMMAND umicom-media-worker-windows-test ${_case})
        set_tests_properties(framework.boot_media.windows_worker.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;media;native;windows;no-devices;worker-lifetime")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.boot_media.windows_worker.${_case}
                TARGETS umicom-media-worker-windows-test)
        endif()
    endforeach()
endif()
