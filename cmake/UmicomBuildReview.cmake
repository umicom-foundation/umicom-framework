# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_umi_review_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::build OR NOT TARGET Umicom::diagnostics)
    message(FATAL_ERROR "Build review requires the canonical build and diagnostics targets")
endif()
if(TARGET umicom_compiler)
    # Preserve the existing compiler API while sharing the established grammar.
    target_link_libraries(umicom_compiler PUBLIC Umicom::diagnostics)
endif()
add_library(umicom_build_review STATIC "${_umi_review_root}/src/build_review/review.c")
add_library(Umicom::build_review ALIAS umicom_build_review)
set_target_properties(umicom_build_review PROPERTIES EXPORT_NAME build_review)
target_include_directories(umicom_build_review PUBLIC
    $<BUILD_INTERFACE:${_umi_review_root}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_build_review PUBLIC Umicom::build)
add_executable(umicom-build-review "${_umi_review_root}/examples/build_review/cli.c")
add_executable(umicom-build-review-example "${_umi_review_root}/examples/build_review/main.c")
foreach(_target IN ITEMS umicom-build-review umicom-build-review-example)
    target_link_libraries(${_target} PRIVATE Umicom::build_review)
endforeach()
foreach(_target IN ITEMS umicom_build_review umicom-build-review umicom-build-review-example)
    set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${_target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${_target})
    endif()
endforeach()
install(TARGETS umicom_build_review EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(TARGETS umicom-build-review umicom-build-review-example
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Learning)
install(FILES "${_umi_review_root}/include/umicom/build/review.h"
    "${_umi_review_root}/include/umicom/build/review_gtk4.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/build COMPONENT Framework)
install(FILES "${_umi_review_root}/docs/learning/read-build-results.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    include("${_umi_review_root}/tests/build_review/CMakeLists.txt")
endif()
unset(_umi_review_root)

# Verify optional module composition independently of the host application.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomBuildConfigurationChecks.cmake")
