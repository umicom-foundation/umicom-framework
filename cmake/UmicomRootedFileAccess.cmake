# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Rooted file access belongs to the existing shared native platform owner.
include_guard(GLOBAL)
target_sources(umicom_platform PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/platform/rooted_files.c")

if(BUILD_TESTING)
    add_executable(umicom-rooted-files-test "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_rooted_files.c")
    target_link_libraries(umicom-rooted-files-test PRIVATE Umicom::platform)
    umicom_apply_warnings(umicom-rooted-files-test)
    umicom_apply_sanitizers(umicom-rooted-files-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-rooted-files-test)
    endif()
    foreach(case roundtrip replace unicode empty limit missing missing-parent invalid invalid-write directory remove staging staging-collision staging-name locked large leaf-link parent-link root-link readonly permissions hardlink)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/rooted-files/${case}")
        add_test(NAME framework.filesystem.rooted.${case} COMMAND umicom-rooted-files-test ${case})
        set_tests_properties(framework.filesystem.rooted.${case} PROPERTIES TIMEOUT 60 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/rooted-files/${case}"
            LABELS "framework;platform;filesystem;workspace;regression")
    endforeach()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomCodingLocalFiles.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/reviewed-workspace-files.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
