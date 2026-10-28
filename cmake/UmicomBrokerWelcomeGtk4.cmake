#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBrokerWelcomeGtk4.cmake
#
# PURPOSE:
#   Build the common broker welcome surface and native ownership regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
target_sources(umicom_ibkr_connection_gtk4 PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/broker_welcome.c")
target_compile_definitions(umicom_ibkr_connection_gtk4 PUBLIC
    UMICOM_HAS_BROKER_WELCOME_GTK4=1 UMICOM_HAS_IBKR_CONNECTION_GTK4=1)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/welcome_gtk4.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/access_gtk4.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/welcome_gtk4.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/access_gtk4.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
if(BUILD_TESTING)
    add_executable(umicom-broker-welcome-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_broker_welcome.c")
    target_link_libraries(umicom-broker-welcome-test PRIVATE Umicom::ibkr_connection_gtk4)
    umicom_ibkr_target(umicom-broker-welcome-test)
    foreach(case modes gateway retained reentrant close-callback close-notify failure optional-profiles)
        add_test(NAME framework.broker_welcome.${case}
            COMMAND umicom-broker-welcome-test ${case})
        set_tests_properties(framework.broker_welcome.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;startup;gtk4;ownership;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-welcome-test)
    endif()
endif()

# Native plan projection must preserve the same mode as the headless service.
if(BUILD_TESTING)
    add_executable(umicom-broker-access-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_access_native.c")
    target_link_libraries(umicom-broker-access-native-test PRIVATE Umicom::ibkr_connection_gtk4)
    umicom_ibkr_target(umicom-broker-access-native-test)
    foreach(case defaults custom invalid consent-reopen launcher-copy parent-retired)
        add_test(NAME framework.broker_access_native.${case}
            COMMAND umicom-broker-access-native-test ${case})
        set_tests_properties(framework.broker_access_native.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;startup;gtk4;ownership;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-access-native-test)
    endif()
endif()

# Exercise asynchronous settings with isolated fixture paths, never user profiles.
if(BUILD_TESTING)
    add_executable(umicom-broker-profile-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_profile_native.c")
    target_link_libraries(umicom-broker-profile-native-test PRIVATE
        Umicom::ibkr_connection_gtk4 Umicom::platform)
    umicom_ibkr_target(umicom-broker-profile-native-test)
    foreach(case save-load missing damaged endpoint-ack stale-fields retired)
        set(fixture "${CMAKE_CURRENT_BINARY_DIR}/broker-profile-native-fixtures/${case}")
        file(MAKE_DIRECTORY "${fixture}")
        add_test(NAME framework.broker_profile_native.${case}
            COMMAND umicom-broker-profile-native-test ${case} "${fixture}")
        set_tests_properties(framework.broker_profile_native.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;settings;gtk4;ownership;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-profile-native-test)
    endif()
endif()
