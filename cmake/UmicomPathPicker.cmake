# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Attach a shared path chooser to the existing native UI owner.
include_guard(GLOBAL)
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/path_picker_gtk4.c")
    if(BUILD_TESTING)
        add_executable(umicom-path-picker-test "${CMAKE_CURRENT_LIST_DIR}/../tests/path_picker/test_path_picker.c")
        # The test substitutes native dialogs while retaining real GTK ownership.
        target_include_directories(umicom-path-picker-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include")
        target_link_libraries(umicom-path-picker-test PRIVATE PkgConfig::UMICOM_FRAMEWORK_GTK4)
        set_target_properties(umicom-path-picker-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-path-picker-test)
        umicom_apply_sanitizers(umicom-path-picker-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-path-picker-test)
        endif()
        foreach(case save open folder edited long close hidden remote cancel)
            add_test(NAME framework.path_picker.${case} COMMAND umicom-path-picker-test ${case})
            set_tests_properties(framework.path_picker.${case} PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;gtk4;files;ownership")
        endforeach()
    endif()
endif()
