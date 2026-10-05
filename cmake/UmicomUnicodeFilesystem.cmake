# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Exercise shared Unicode paths using real isolated files and native readers.
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-unicode-filesystem-test "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_unicode_filesystem.c")
    target_link_libraries(umicom-unicode-filesystem-test PRIVATE Umicom::platform)
    umicom_apply_warnings(umicom-unicode-filesystem-test)
    umicom_apply_sanitizers(umicom-unicode-filesystem-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-unicode-filesystem-test)
    endif()
    foreach(case application-paths roundtrip append copy rename directories metadata walk cancel remove current temporary invalid reparse)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/unicode-filesystem/${case}")
        add_test(NAME framework.filesystem.unicode.${case} COMMAND umicom-unicode-filesystem-test ${case})
        set_tests_properties(framework.filesystem.unicode.${case} PROPERTIES TIMEOUT 60 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/unicode-filesystem/${case}"
            LABELS "framework;platform;filesystem;unicode;regression")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/unicode-project-paths.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
