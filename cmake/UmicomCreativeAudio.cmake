# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Extend the existing creative owner; no second asset or persistence service.
include_guard(GLOBAL)
set(_audio_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_creative_workspace PRIVATE
    "${_audio_root}/src/creative_workspace/audio.c"
    "${_audio_root}/src/creative_workspace/audio_file.c")
add_executable(umicom-audio-clip
    "${_audio_root}/examples/creative_audio/main.c"
    "${_audio_root}/examples/creative_audio/lesson.c")
target_link_libraries(umicom-audio-clip PRIVATE Umicom::creative_workspace)
umicom_creative_configure_target(umicom-audio-clip)
if(MINGW)
    target_link_options(umicom-audio-clip PRIVATE -municode)
endif()
install(TARGETS umicom-audio-clip RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
if(TARGET umicom_creative_workspace_gtk4)
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${_audio_root}/adapters/gtk4/creative_audio_gtk4.c")
    # Products share preview ownership and playback lifecycle through Framework.
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${_audio_root}/adapters/gtk4/creative_audition_gtk4.c")
    install(FILES "${_audio_root}/include/umicom/ui/gtk4/creative_audition.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
    install(FILES "${_audio_root}/include/umicom/ui/gtk4/creative_audio.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
if(BUILD_TESTING)
    # include, not add_subdirectory: also valid during deferred composition.
    include("${_audio_root}/tests/creative_audio/CMakeLists.txt")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-audio-clip)
    endif()
endif()
install(FILES "${_audio_root}/docs/learning/CREATIVE_AUDIO.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
unset(_audio_root)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/listen-to-audio-previews.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
