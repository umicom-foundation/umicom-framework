# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Coverage is a data observation; it requires no renderer or brokerage request.
target_sources(umicom_ibkr_connection PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/bar_coverage.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/bar_coverage.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-bar-coverage-test "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_bar_coverage.c")
    target_link_libraries(umicom-bar-coverage-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-bar-coverage-test)
    add_test(NAME framework.ibkr_bar_coverage.arithmetic COMMAND umicom-bar-coverage-test)
    set_tests_properties(framework.ibkr_bar_coverage.arithmetic PROPERTIES TIMEOUT 20 LABELS "framework;broker;chart;regression")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-bar-coverage-test)
    endif()
endif()
