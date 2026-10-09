# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Verify the reusable lesson library against the resources shipped with Framework.
include_guard(GLOBAL)
if(BUILD_TESTING AND TARGET umicom_developer)
    add_executable(umicom-learning-library-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/learning_library/test_library.c")
    target_link_libraries(umicom-learning-library-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-learning-library-test)
    umicom_apply_sanitizers(umicom-learning-library-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-library-test)
    endif()
    foreach(case framework docs learning invalid)
        add_test(NAME framework.learning_library.${case}
            COMMAND umicom-learning-library-test "${CMAKE_CURRENT_LIST_DIR}/.." "${case}")
        set_tests_properties(framework.learning_library.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;teacher;studio;regression")
    endforeach()
endif()
