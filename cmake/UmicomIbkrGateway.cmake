#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomIbkrGateway.cmake
#
# PURPOSE:
#   Register shared Gateway discovery, native setup and regression coverage.
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
    "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ibkr_gateway.c"
    "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ibkr_gateway_discovery.c")
target_compile_definitions(umicom_ibkr_connection_gtk4 PUBLIC UMICOM_HAS_IBKR_GATEWAY_GTK4=1)
foreach(component Framework FrameworkSDK)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/gateway_gtk4.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT ${component})
endforeach()
if(BUILD_TESTING)
    add_executable(umicom-ibkr-gateway-discovery-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_gateway_discovery.c")
    target_link_libraries(umicom-ibkr-gateway-discovery-test PRIVATE Umicom::ibkr_connection_gtk4)
    umicom_ibkr_target(umicom-ibkr-gateway-discovery-test)
    foreach(case direct version custom duplicates limit missing relative uninstaller uri missing-custom cancel)
        add_test(NAME framework.ibkr_gateway.discovery.${case}
            COMMAND umicom-ibkr-gateway-discovery-test ${case})
        set_tests_properties(framework.ibkr_gateway.discovery.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;broker;gateway;regression")
    endforeach()
    add_executable(umicom-ibkr-gateway-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_gateway_gtk4.c")
    target_link_libraries(umicom-ibkr-gateway-native-test PRIVATE Umicom::ibkr_connection_gtk4)
    umicom_ibkr_target(umicom-ibkr-gateway-native-test)
    foreach(case empty custom-invalid pending-close notify-close retained)
        add_test(NAME framework.ibkr_gateway.native.${case}
            COMMAND umicom-ibkr-gateway-native-test ${case})
        set_tests_properties(framework.ibkr_gateway.native.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;gateway;gtk4;ownership")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-gateway-discovery-test)
        umicom_register_validation_target(umicom-ibkr-gateway-native-test)
    endif()
endif()
