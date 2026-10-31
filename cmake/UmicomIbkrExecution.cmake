#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomIbkrExecution.cmake
#
# PURPOSE:
#   Register reviewed orders, exact instrument discovery and local watchlists.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
set(_umi_execution_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_ibkr_connection PRIVATE
    "${_umi_execution_root}/src/ibkr_connection/order_execution.c"
    "${_umi_execution_root}/src/ibkr_connection/order_wire.c"
    "${_umi_execution_root}/src/ibkr_connection/submission_observation.c"
    "${_umi_execution_root}/src/ibkr_connection/instrument_search.c"
    "${_umi_execution_root}/src/ibkr_connection/watchlist.c")
foreach(_header IN ITEMS order_execution watchlist)
    install(FILES "${_umi_execution_root}/include/umicom/broker_connectivity/${_header}.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
    install(FILES "${_umi_execution_root}/include/umicom/broker_connectivity/${_header}.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
endforeach()
if(BUILD_TESTING)
    foreach(_feature IN ITEMS order_execution watchlist instrument_search)
        string(REPLACE "_" "-" _slug "${_feature}")
        set(_target "umicom-ibkr-${_slug}-test")
        add_executable(${_target} "${_umi_execution_root}/tests/ibkr_connection/test_${_feature}.c")
        target_include_directories(${_target} PRIVATE "${_umi_execution_root}/src/ibkr_connection")
        target_link_libraries(${_target} PRIVATE Umicom::ibkr_connection)
        umicom_ibkr_target(${_target})
        if(_feature STREQUAL "order_execution")
            set(_cases authority completion invalid-price invalid-quantity stale-metadata ambiguous
                unsupported-bond route edit-invalidates expired queue-full single-use namespace
                cancel disconnect rejection read-queue wire-164 wire-166 wire-169 wire-176
                stale-account account-expires reconcile foreign-client identity-mismatch status-only terminal-sticky)
        elseif(_feature STREQUAL "watchlist")
            set(_cases duplicate route invalid capacity remove unterminated)
        else()
            set(_cases invalid capacity futures busy bond malformed)
        endif()
        foreach(_case IN LISTS _cases)
            add_test(NAME framework.ibkr_${_feature}.${_case} COMMAND ${_target} ${_case})
            set_tests_properties(framework.ibkr_${_feature}.${_case} PROPERTIES
                TIMEOUT 20 LABELS "framework;broker;regression")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
endif()
unset(_umi_execution_root)
