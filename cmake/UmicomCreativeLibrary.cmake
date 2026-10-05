# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Collections extend the existing asset owner; products share storage and GUI
# lifetime rules instead of each introducing a second importer or file format.
target_sources(umicom_creative_workspace PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/asset_library.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/asset_library_archive.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/asset_library_file.c")
if(TARGET umicom_creative_workspace_gtk4)
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/creative_library_gtk4.c")
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/creative_library.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/creative-asset-libraries.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)

if(BUILD_TESTING)
    add_executable(umicom-creative-library-core-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_library/test_library.c")
    target_link_libraries(umicom-creative-library-core-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-creative-library-core-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-creative-library-core-test)
    endif()
    foreach(case ownership independent duplicate alias invalid-id empty-id long-id case-sensitive missing reorder detach output-live title unicode-title invalid-title count-limit payload-limit exact-limit empty-assets invalid-arguments)
        add_test(NAME framework.creative_library.core.${case} COMMAND umicom-creative-library-core-test ${case})
        set_tests_properties(framework.creative_library.core.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative-library;core;regression")
    endforeach()
endif()

if(BUILD_TESTING)
    add_executable(umicom-creative-library-archive-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_library/test_archive.c")
    target_link_libraries(umicom-creative-library-archive-test PRIVATE Umicom::creative_workspace Umicom::native_launcher)
    umicom_creative_configure_target(umicom-creative-library-archive-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-creative-library-archive-test)
    endif()
    foreach(case roundtrip empty-library empty-payload purposes deterministic independent-fixture unicode-title longest-fields chunked cancelled-encode cancelled-decode live-output invalid-arguments invalid-limit payload-limit magic schema reserved count title-size title-nul title-utf8 total-size record-reserved id-size id-nul id-invalid duplicate-id inner-size inner-digest outer-digest trailing truncated record-count)
        add_test(NAME framework.creative_library.archive.${case} COMMAND umicom-creative-library-archive-test ${case})
        set_tests_properties(framework.creative_library.archive.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative-library;archive;regression")
    endforeach()
endif()

if(BUILD_TESTING AND (WIN32 OR UNIX) AND NOT EMSCRIPTEN)
    add_executable(umicom-creative-library-file-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_library/test_file.c")
    target_link_libraries(umicom-creative-library-file-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-creative-library-file-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-creative-library-file-test)
    endif()
    foreach(case roundtrip existing cancelled-save cancelled-load relative-save relative-load missing directory payload-limit source-privacy unicode-path truncated corrupt byte-equality live-output)
        add_test(NAME framework.creative_library.file.${case} COMMAND umicom-creative-library-file-test ${case})
        set_tests_properties(framework.creative_library.file.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative-library;file;regression")
    endforeach()
endif()

if(BUILD_TESTING AND (WIN32 OR UNIX) AND NOT EMSCRIPTEN AND TARGET Umicom::creative_workspace_gtk4)
    add_executable(umicom-creative-library-gtk-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_library/test_gtk4.c")
    target_link_libraries(umicom-creative-library-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
    umicom_creative_configure_target(umicom-creative-library-gtk-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-creative-library-gtk-test)
    endif()
    foreach(case import duplicate reorder remove-undo save-reopen corrupt-retains cancel-import cancel-open busy open-approval approval-invalidation export existing-export editor-copy independent retained-control close-busy title)
        add_test(NAME framework.creative_library.gtk.${case} COMMAND umicom-creative-library-gtk-test ${case})
        set_tests_properties(framework.creative_library.gtk.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative-library;gtk;regression")
    endforeach()
endif()

if(BUILD_TESTING AND (WIN32 OR UNIX) AND NOT EMSCRIPTEN AND TARGET Umicom::creative_workspace_gtk4)
    add_executable(umicom-creative-library-profiles-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_library/test_profiles_gtk4.c")
    target_link_libraries(umicom-creative-library-profiles-test PRIVATE Umicom::creative_workspace_gtk4)
    umicom_creative_configure_target(umicom-creative-library-profiles-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-creative-library-profiles-test)
    endif()
    foreach(case media music cad kitchen games web-studio mobile-studio)
        add_test(NAME framework.creative_library.profiles.${case} COMMAND umicom-creative-library-profiles-test ${case})
        set_tests_properties(framework.creative_library.profiles.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative-library;profiles;regression")
    endforeach()
endif()
