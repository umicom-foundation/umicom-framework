#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBrokerAccess.cmake
#
# PURPOSE:
#   Compose credential-free entry planning over the existing broker owner.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/access_plan.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/access_plan.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/access_plan.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
if(BUILD_TESTING)
    add_executable(umicom-broker-access-plan-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_access_plan.c")
    target_link_libraries(umicom-broker-access-plan-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-broker-access-plan-test)
    foreach(case guest paper live-consent gateway invalid endpoint guidance)
        add_test(NAME framework.broker_access.${case}
            COMMAND umicom-broker-access-plan-test ${case})
        set_tests_properties(framework.broker_access.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;broker;startup;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-access-plan-test)
    endif()
endif()

# The learning executable only inspects plans; it opens no network transport.
add_executable(umicom-broker-access-example
    "${CMAKE_CURRENT_LIST_DIR}/../examples/ibkr_connection/access_plan.c")
target_link_libraries(umicom-broker-access-example PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-broker-access-example)

# Persistence is shared by native and future web/mobile clients.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomBrokerProfiles.cmake")
