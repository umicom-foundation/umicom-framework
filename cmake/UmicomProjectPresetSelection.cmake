# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Preset metadata belongs to the developer service. Applications reuse its
# bounded reader and GTK settings presenter without implementing JSON parsing.
include_guard(GLOBAL)
target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/preset_catalogue.c")
target_link_libraries(umicom_developer PUBLIC Umicom::build)

if(BUILD_TESTING)
    add_executable(umicom-project-preset-catalogue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/project_presets/test_catalogue.c")
    target_link_libraries(umicom-project-preset-catalogue-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-project-preset-catalogue-test)
    umicom_apply_sanitizers(umicom-project-preset-catalogue-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-preset-catalogue-test)
    endif()
    foreach(case syntax trailing root missing-version fraction-version overflow-version duplicate-field group-type row-type missing-name empty-name duplicate-name-field duplicate-name control-name nul-name surrogate hidden-type condition-type inherit-type include-type reference-type metadata-type unknown-invalid absent arguments embedded-nul byte-limit name-limit row-limit cross-file malformed-user user-only empty metadata unicode-owned stages select atomic)
        add_test(NAME framework.project_presets.catalogue.${case}
            COMMAND umicom-project-preset-catalogue-test ${case})
        set_tests_properties(framework.project_presets.catalogue.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;developer;presets;regression")
    endforeach()
endif()

# GTK may be provided by a parent later in configuration. Attach once after
# that owner exists, as the shared developer-dialog module already does.
function(umicom_attach_project_preset_native_checks)
    if(NOT BUILD_TESTING OR NOT TARGET umicom_ui_gtk4 OR TARGET umicom-project-preset-gtk4-test)
        return()
    endif()
    add_executable(umicom-project-preset-gtk4-test
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/project_presets/test_gtk4.c")
    target_link_libraries(umicom-project-preset-gtk4-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-project-preset-gtk4-test)
    umicom_apply_sanitizers(umicom-project-preset-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-preset-gtk4-test)
    endif()
    foreach(case includes-use includes-origin includes-inherited includes-missing includes-cancel includes-retained changed-back no-auto-read reentrant-load model-destroy missing user-only relative-folder pending-destroy pending-parent pending-cancel refresh-invalid reentrant-detail hidden false-condition legacy changed-folder reentrant-entry stages-apply)
        add_test(NAME framework.project_presets.gtk4.${case}
            COMMAND umicom-project-preset-gtk4-test ${case})
        set_tests_properties(framework.project_presets.gtk4.${case} PROPERTIES
            TIMEOUT 40 SKIP_RETURN_CODE 77 LABELS "framework;developer;presets;gtk4;ownership;regression")
    endforeach()
endfunction()
umicom_attach_project_preset_native_checks()
if(NOT TARGET umicom_ui_gtk4)
    cmake_language(DEFER CALL umicom_attach_project_preset_native_checks)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomPresetGraph.cmake")
