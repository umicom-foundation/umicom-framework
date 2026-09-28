#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Extend the canonical AI Workspace; applications acquire no competing runtime.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT TARGET umicom_ai_workspace)
    message(FATAL_ERROR "AI evidence requires the canonical AI workspace target")
endif()
set(_umi_ai_evidence_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_ai_workspace PRIVATE
    "${_umi_ai_evidence_root}/src/ai_workspace/evidence_selection.c"
    "${_umi_ai_evidence_root}/src/ai_workspace/evidence_review.c"
    "${_umi_ai_evidence_root}/src/ai_workspace/evidence_render.c")
add_executable(umicom-ai-evidence
    "${_umi_ai_evidence_root}/examples/ai_evidence/main.c"
    "${_umi_ai_evidence_root}/examples/ai_evidence/lesson.c")
target_link_libraries(umicom-ai-evidence PRIVATE Umicom::ai_workspace)
umicom_ai_workspace_configure_target(umicom-ai-evidence)
install(TARGETS umicom-ai-evidence RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT Framework)
install(FILES "${_umi_ai_evidence_root}/docs/learning/ai-evidence.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
if(BUILD_TESTING)
    # Include the definitions, not a deferred add_subdirectory. Source paths
    # are resolved beside the list file itself, as in the repaired components.
    include("${_umi_ai_evidence_root}/tests/ai_evidence/CMakeLists.txt")
endif()
if(COMMAND umicom_register_validation_target)
    foreach(target umicom-ai-evidence umicom-ai-evidence-test umicom-ai-evidence-http-test
                   umicom-ai-evidence-allocation-test umicom-ai-evidence-gtk-test)
        if(TARGET ${target})
            umicom_register_validation_target(${target})
        endif()
    endforeach()
endif()
unset(_umi_ai_evidence_root)
