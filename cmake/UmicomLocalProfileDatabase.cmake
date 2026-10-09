# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING AND SQLite3_FOUND)
    add_executable(umicom-local-profile-database-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/local_profile/test_database.c")
    target_link_libraries(umicom-local-profile-database-test PRIVATE Umicom::security Umicom::data)
    umicom_apply_warnings(umicom-local-profile-database-test)
    umicom_apply_sanitizers(umicom-local-profile-database-test)
    foreach(case reopen duplicate namespace corrupt verifier remove)
        add_test(NAME framework.local_profile.database.${case} COMMAND "${CMAKE_COMMAND}"
            "-DTEST_BINARY=$<TARGET_FILE:umicom-local-profile-database-test>"
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}" "-DTEST_CASE=${case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/local_profile/database_fixture.cmake")
        set_tests_properties(framework.local_profile.database.${case} PROPERTIES
            TIMEOUT 90 SKIP_REGULAR_EXPRESSION "UMICOM_PROFILE_DATABASE_UNAVAILABLE"
            LABELS "framework;security;local-profile;sqlite;native")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-local-profile-database-test)
    endif()
endif()
