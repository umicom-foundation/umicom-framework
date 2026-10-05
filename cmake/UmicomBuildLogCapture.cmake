# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Native log ownership belongs to platform; project orchestration reuses it.
include_guard(GLOBAL)
if(BUILD_TESTING)
    foreach(kind file session io_failure)
        set(target "umicom-build-log-${kind}-test")
        add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/build_log/test_${kind}.c")
        if(kind STREQUAL "io_failure")
            # This isolated target compiles its fault-injected writer itself.
            target_link_libraries(${target} PRIVATE Umicom::base)
        else()
            target_link_libraries(${target} PRIVATE Umicom::build)
        endif()
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endforeach()
    foreach(case binary unicode large collision empty destroy invalid missing-parent symlink)
        add_test(NAME framework.build_log.file.${case} COMMAND umicom-build-log-file-test ${case})
        set_tests_properties(framework.build_log.file.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;build;log;filesystem;regression")
    endforeach()
    foreach(case complete cancel failure destroy collision legacy unlogged-next)
        add_test(NAME framework.build_log.session.${case} COMMAND umicom-build-log-session-test ${case})
        set_tests_properties(framework.build_log.session.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;build;log;regression")
    endforeach()
    foreach(case partial flush close)
        add_test(NAME framework.build_log.io_failure.${case} COMMAND umicom-build-log-io_failure-test ${case})
        set_tests_properties(framework.build_log.io_failure.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;build;log;fault-injection;regression")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/saving-build-logs.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
