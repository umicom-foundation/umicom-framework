#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBrokerProfiles.cmake
#
# PURPOSE:
#   Compose local broker settings and their parser, persistence and native checks.
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
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/access_profiles.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/access_profile_store.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/access_profiles.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/access_profiles.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
if(BUILD_TESTING)
    add_executable(umicom-broker-profiles-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_access_profiles.c")
    target_link_libraries(umicom-broker-profiles-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-broker-profiles-test)
    foreach(case defaults roundtrip capacity truncation binary malformed oversize invalid)
        add_test(NAME framework.broker_profiles.${case}
            COMMAND umicom-broker-profiles-test ${case})
        set_tests_properties(framework.broker_profiles.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;broker;settings;regression")
    endforeach()
    add_executable(umicom-broker-profile-store-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_access_profile_store.c")
    target_link_libraries(umicom-broker-profile-store-test PRIVATE
        Umicom::ibkr_connection Umicom::platform)
    umicom_ibkr_target(umicom-broker-profile-store-test)
    foreach(case missing preserve damaged busy invalid)
        set(fixture "${CMAKE_CURRENT_BINARY_DIR}/broker-profile-fixtures/${case}")
        file(MAKE_DIRECTORY "${fixture}")
        add_test(NAME framework.broker_profile_store.${case}
            COMMAND umicom-broker-profile-store-test ${case} "${fixture}")
        set_tests_properties(framework.broker_profile_store.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;broker;settings;filesystem;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-profiles-test)
        umicom_register_validation_target(umicom-broker-profile-store-test)
    endif()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/local-broker-settings.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
