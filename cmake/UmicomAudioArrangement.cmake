# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Arrangement editing, JSON and rendering belong to the creative owner. The
# existing decoder provides one validation policy for all audio source imports.
target_sources(umicom_creative_workspace PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/audio_arrangement.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/audio_arrangement_document.c")
target_link_libraries(umicom_creative_workspace PRIVATE Umicom::developer Umicom::ai)
if(TARGET umicom_creative_workspace_gtk4)
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/audio_arrangement_gtk4.c")
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/audio_arrangement.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/audio-arrangements.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)

if(BUILD_TESTING)
    add_executable(umicom-audio-arrangement-render-test "${CMAKE_CURRENT_LIST_DIR}/../tests/audio_arrangement/test_render.c")
    target_link_libraries(umicom-audio-arrangement-render-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-audio-arrangement-render-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-audio-arrangement-render-test)
    endif()
    foreach(case mono stereo gap overlap signed-clipping cancellation-before-saturation attenuation silence trim fades negative-fades source-reuse sample-rate missing-source invalid-wave outside-source output-limit cancelled live-output empty-plan invalid-plan duplicate-placement explicit-replace remove trim-ramp negative-clipping)
        add_test(NAME framework.audio_arrangement.render.${case} COMMAND umicom-audio-arrangement-render-test ${case})
        set_tests_properties(framework.audio_arrangement.render.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;audio-arrangement;render;regression")
    endforeach()
endif()

if(BUILD_TESTING)
    add_executable(umicom-audio-arrangement-document-test "${CMAKE_CURRENT_LIST_DIR}/../tests/audio_arrangement/test_document.c")
    target_link_libraries(umicom-audio-arrangement-document-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-audio-arrangement-document-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-audio-arrangement-document-test)
    endif()
    foreach(case roundtrip empty unicode duplicate-key unknown-field missing-field fractional negative overflow duplicate-id invalid-range trailing cancelled malformed live-export over-capacity)
        add_test(NAME framework.audio_arrangement.document.${case} COMMAND umicom-audio-arrangement-document-test ${case})
        set_tests_properties(framework.audio_arrangement.document.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;audio-arrangement;document;regression")
    endforeach()
endif()

if(BUILD_TESTING AND TARGET Umicom::creative_workspace_gtk4 AND (WIN32 OR UNIX) AND NOT EMSCRIPTEN)
    add_executable(umicom-audio-arrangement-gtk-test "${CMAKE_CURRENT_LIST_DIR}/../tests/audio_arrangement/test_gtk4.c")
    target_link_libraries(umicom-audio-arrangement-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
    umicom_creative_configure_target(umicom-audio-arrangement-gtk-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-audio-arrangement-gtk-test)
    endif()
    foreach(case add duplicate replace remove invalid-retains render export existing-export edit-invalidates library-invalidates cancelled-render save-open open-approval path-approval failed-open retained-control close-busy)
        add_test(NAME framework.audio_arrangement.gtk.${case} COMMAND umicom-audio-arrangement-gtk-test ${case})
        set_tests_properties(framework.audio_arrangement.gtk.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;audio-arrangement;gtk;regression")
    endforeach()
endif()

# The command is a thin native host for the same library/recipe/render services.
if((WIN32 OR UNIX) AND NOT EMSCRIPTEN)
    add_executable(umicom-audio-arrangement
        "${CMAKE_CURRENT_LIST_DIR}/../examples/audio_arrangement/main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/audio_arrangement/command.c")
    target_link_libraries(umicom-audio-arrangement PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-audio-arrangement)
    if(MINGW)
        target_link_options(umicom-audio-arrangement PRIVATE -municode)
    endif()
    install(TARGETS umicom-audio-arrangement RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    if(BUILD_TESTING)
        add_executable(umicom-audio-arrangement-command-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/audio_arrangement/test_command.c"
            "${CMAKE_CURRENT_LIST_DIR}/../examples/audio_arrangement/command.c")
        target_link_libraries(umicom-audio-arrangement-command-test PRIVATE Umicom::creative_workspace)
        umicom_creative_configure_target(umicom-audio-arrangement-command-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-audio-arrangement)
            umicom_register_validation_target(umicom-audio-arrangement-command-test)
        endif()
        foreach(case help inspect render unicode existing missing-source invalid-recipe invalid-command relative-output)
            add_test(NAME framework.audio_arrangement.command.${case} COMMAND umicom-audio-arrangement-command-test ${case})
            set_tests_properties(framework.audio_arrangement.command.${case} PROPERTIES
                TIMEOUT 30 LABELS "framework;audio-arrangement;native;command;regression")
        endforeach()
    endif()
endif()
