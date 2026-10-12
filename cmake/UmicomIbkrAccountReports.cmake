# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/account_query.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/account_query.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
# Application guides belong to the shared docs/guides installation in the
# root CMakeLists.txt. Keep this former learning-folder rule disabled for
# source review; new user guides should use the shared installation instead.
if(FALSE)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/broker-account-reports.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
endif()
if(BUILD_TESTING)
    add_executable(umicom-ibkr-account-report-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_account_report.c")
    target_include_directories(umicom-ibkr-account-report-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-account-report-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-account-report-test)
    foreach(case tags short-output bad-currency bad-kind before-ready unlisted backwards
        queue queue-second margin currency read-once values filter late cancel)
        add_test(NAME framework.ibkr_account_report.${case}
            COMMAND umicom-ibkr-account-report-test ${case})
        set_tests_properties(framework.ibkr_account_report.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;broker;accounts;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-account-report-test)
    endif()
endif()
