# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-release-inspector-test test_main.c pe_fixture.c test_pe.c test_files.c test_paths.c test_pack.c)
target_include_directories(umicom-release-inspector-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/setup_centre")
target_link_libraries(umicom-release-inspector-test PRIVATE Umicom::release_inspector)
umicom_apply_warnings(umicom-release-inspector-test)
umicom_apply_sanitizers(umicom-release-inspector-test)
set(_cases
    release.sourceview_missing release.sourceview_present
    bootstrap.clean bootstrap.changed bootstrap.missing bootstrap.private_import
    pe.valid pe.normal pe.delay pe.pe32 pe.truncated pe.signature pe.offset pe.optional
    pe.sections pe.headers pe.image_size pe.alignment pe.overlap_raw pe.overlap_rva
    pe.virtual_tail pe.bad_import_name pe.no_terminator pe.delay_legacy pe.unknown_machine
    pe.flags pe.entry_outside pe.directory_overflow pe.directory_count pe.missing_iat
    pe.null_output pe.mutations pe.name_capacity
    paths.suffix_characters paths.device_unicode paths.valid_extensions catalogue.stream_refusal
    release.clean release.missing_dll release.wrong_machine release.invalid_image release.bad_type
    release.unselected_dependency release.delay_missing release.system_deferred release.private_shadow
    release.unknown_system release.resource_missing release.resource_present release.module_nested
    release.entry_nested release.changed release.catalogue_changed release.cancel release.cancel_mid
    release.no_outputs release.unselected_missing installation.clean installation.tamper
    installation.personal_files installation.missing_receipt summary.capacity cli.report
    cli.repeat_report cli.malformed_options)
foreach(_case IN LISTS _cases)
    add_test(NAME framework.release_inspector.${_case} COMMAND umicom-release-inspector-test "${_case}")
    set_tests_properties(framework.release_inspector.${_case} PROPERTIES TIMEOUT 90
        LABELS "framework;release;native;regression")
endforeach()
add_test(NAME framework.release_inspector.cli_help COMMAND umicom-release-inspect --help)
add_test(NAME framework.release_inspector.cli_self_test COMMAND umicom-release-inspect --self-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-release-inspector-test)
endif()
# Windows application loading is a separate target-platform check, added below.

# These images contain real linker-produced PE records, but are NEVER executed.
# Missing Clang/LLD means these two cross-format checks are not registered; it
# does not turn synthetic fixtures into evidence of a Windows build.
find_program(_umi_format_clang NAMES clang)
find_program(_umi_format_lld NAMES lld-link)
if(_umi_format_clang AND _umi_format_lld AND NOT WIN32)
    set(_format_root "${CMAKE_CURRENT_BINARY_DIR}/linked-pe")
    add_custom_command(
        OUTPUT "${_format_root}/consumer.exe" "${_format_root}/delayed.exe" "${_format_root}/umicom-practice-storage.dll" "${_format_root}/bootstrap.exe"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${_format_root}"
        COMMAND "${_umi_format_clang}" --target=x86_64-pc-windows-msvc -ffreestanding -fno-stack-protector
            -c "${CMAKE_CURRENT_SOURCE_DIR}/linked_pe/storage.c" -o "${_format_root}/storage.obj"
        COMMAND "${_umi_format_lld}" /dll /entry:DllMain /nodefaultlib
            "/out:${_format_root}/umicom-practice-storage.dll" "/implib:${_format_root}/storage.lib" "${_format_root}/storage.obj"
        COMMAND "${_umi_format_clang}" --target=x86_64-pc-windows-msvc -ffreestanding -fno-stack-protector
            -c "${CMAKE_CURRENT_SOURCE_DIR}/linked_pe/consumer.c" -o "${_format_root}/consumer.obj"
        COMMAND "${_umi_format_lld}" /entry:mainCRTStartup /nodefaultlib /subsystem:console
            "/out:${_format_root}/consumer.exe" "${_format_root}/consumer.obj" "${_format_root}/storage.lib"
        COMMAND "${_umi_format_clang}" --target=x86_64-pc-windows-msvc -ffreestanding -fno-stack-protector
            -DUMICOM_FORMAT_DELAY=1 -c "${CMAKE_CURRENT_SOURCE_DIR}/linked_pe/consumer.c" -o "${_format_root}/delayed.obj"
        COMMAND "${_umi_format_lld}" /entry:mainCRTStartup /nodefaultlib /subsystem:console
            /delayload:umicom-practice-storage.dll "/out:${_format_root}/delayed.exe"
            "${_format_root}/delayed.obj" "${_format_root}/storage.lib"
        COMMAND "${_umi_format_clang}" --target=x86_64-pc-windows-msvc -ffreestanding -fno-stack-protector
            -c "${CMAKE_CURRENT_SOURCE_DIR}/linked_pe/bootstrap.c" -o "${_format_root}/bootstrap.obj"
        COMMAND "${_umi_format_lld}" /entry:mainCRTStartup /nodefaultlib /subsystem:windows
            "/out:${_format_root}/bootstrap.exe" "${_format_root}/bootstrap.obj"
        DEPENDS linked_pe/storage.c linked_pe/consumer.c linked_pe/bootstrap.c VERBATIM)
    add_custom_target(umicom-release-linked-pe-fixtures
        DEPENDS "${_format_root}/consumer.exe" "${_format_root}/delayed.exe" "${_format_root}/umicom-practice-storage.dll")
    add_dependencies(umicom-release-inspector-test umicom-release-linked-pe-fixtures)
    add_test(NAME framework.release_inspector.linked_pe.normal
        COMMAND umicom-release-inspector-test linked-pe "${_format_root}/consumer.exe" normal)
    add_test(NAME framework.release_inspector.linked_pe.delay
        COMMAND umicom-release-inspector-test linked-pe "${_format_root}/delayed.exe" delay)
    add_test(NAME framework.release_inspector.linked_pe.pack_install
        COMMAND umicom-release-inspector-test pack-linked "${_format_root}/consumer.exe"
            "${_format_root}/umicom-practice-storage.dll" "${_format_root}/bootstrap.exe")
    set_tests_properties(framework.release_inspector.linked_pe.pack_install PROPERTIES TIMEOUT 60
        LABELS "framework;release;native-pack;format-only;not-windows-runtime")
    set_tests_properties(framework.release_inspector.linked_pe.normal framework.release_inspector.linked_pe.delay
        PROPERTIES TIMEOUT 30 LABELS "framework;release;format-only;not-windows-runtime")
else()
    message(STATUS "Clang/LLD cross-format fixtures not registered on this host")
endif()

if(WIN32 AND TARGET umicom-release-notes AND TARGET Umicom::setup_process)
    add_executable(umicom-release-windows-install-test test_windows_install.c)
    target_include_directories(umicom-release-windows-install-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../../src/setup_centre")
    target_link_libraries(umicom-release-windows-install-test PRIVATE Umicom::release_inspector Umicom::setup_process)
    if(MINGW)
        target_link_options(umicom-release-windows-install-test PRIVATE -municode -static-libgcc)
    endif()
    umicom_apply_warnings(umicom-release-windows-install-test)
    umicom_apply_sanitizers(umicom-release-windows-install-test)
    add_dependencies(umicom-release-windows-install-test umicom-release-notes umicom-release-notes-model)
    foreach(_case IN ITEMS installed_startup missing_dll missing_resource)
        add_test(NAME framework.release_inspector.windows.${_case}
            COMMAND umicom-release-windows-install-test "${_case}"
                "$<TARGET_FILE:umicom-release-notes>" "$<TARGET_FILE:umicom-release-notes-model>"
                "${CMAKE_CURRENT_LIST_DIR}/../../examples/release_notes/example-note.txt")
        set_tests_properties(framework.release_inspector.windows.${_case} PROPERTIES
            TIMEOUT 90 LABELS "framework;release;windows;real-native-startup")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-release-windows-install-test)
    endif()
endif()

add_executable(umicom-release-notes-model-test test_notes_model.c)
if(TARGET umicom-release-notes-model)
    target_link_libraries(umicom-release-notes-model-test PRIVATE umicom-release-notes-model)
else()
    target_sources(umicom-release-notes-model-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../examples/release_notes/model.c")
endif()
umicom_apply_warnings(umicom-release-notes-model-test)
umicom_apply_sanitizers(umicom-release-notes-model-test)
add_test(NAME framework.release_inspector.notes_model COMMAND umicom-release-notes-model-test)
set_tests_properties(framework.release_inspector.notes_model PROPERTIES TIMEOUT 30 LABELS "framework;release;example")
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-release-notes-model-test)
endif()
