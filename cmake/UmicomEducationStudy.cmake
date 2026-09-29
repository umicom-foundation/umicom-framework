# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Additional read-only projections use the existing Education authority.
include_guard(GLOBAL)
if(NOT TARGET Umicom::education_workspace)
    message(FATAL_ERROR "Study tools require the canonical Education workspace")
endif()
set(_umi_study_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_education_workspace PRIVATE
    "${_umi_study_root}/src/education_workspace/study.c"
    "${_umi_study_root}/src/education_workspace/library.c")
add_executable(umicom-study
    "${_umi_study_root}/examples/education_study/main.c"
    "${_umi_study_root}/examples/education_study/lesson.c")
target_link_libraries(umicom-study PRIVATE Umicom::education_workspace)
umicom_education_configure_target(umicom-study)
install(TARGETS umicom-study RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT Learning)
install(FILES "${_umi_study_root}/docs/learning/EDUCATION_STUDY.html"
    "${_umi_study_root}/docs/learning/LEARNING_LIBRARY.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Learning)
if(TARGET umicom_education_workspace_gtk4)
    target_sources(umicom_education_workspace_gtk4 PRIVATE
        "${_umi_study_root}/adapters/gtk4/education_study_gtk4.c")
endif()
if(BUILD_TESTING)
    # Include definitions rather than create a subdirectory from a deferred call.
    include("${_umi_study_root}/tests/education_study/CMakeLists.txt")
endif()
unset(_umi_study_root)
