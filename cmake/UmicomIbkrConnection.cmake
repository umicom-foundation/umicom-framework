# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# The caller supplies the canonical broker mapping owner; no parallel OMS.
include_guard(GLOBAL)
include(GNUInstallDirs)
if(NOT TARGET Umicom::trading)
    message(FATAL_ERROR "IBKR connection requires canonical Umicom::trading")
endif()
set(_umi_ibkr_root "${CMAKE_CURRENT_LIST_DIR}/..")
function(umicom_ibkr_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()
add_library(umicom_ibkr_connection STATIC
    "${_umi_ibkr_root}/src/ibkr_connection/session.c"
    "${_umi_ibkr_root}/src/ibkr_connection/protocol.c"
    "${_umi_ibkr_root}/src/ibkr_connection/quotes.c"
    "${_umi_ibkr_root}/src/ibkr_connection/contract_details.c"
    "${_umi_ibkr_root}/src/ibkr_connection/contract_route.c"
    "${_umi_ibkr_root}/src/ibkr_connection/market_rule.c"
    "${_umi_ibkr_root}/src/ibkr_connection/fill_policy.c"
    "${_umi_ibkr_root}/src/ibkr_connection/position_review.c"
    "${_umi_ibkr_root}/src/ibkr_connection/network.c")
# Captured report formatting shares the existing connection owner and CSV rules.
target_sources(umicom_ibkr_connection PRIVATE "${_umi_ibkr_root}/src/ibkr_connection/observation_export.c")
add_library(Umicom::ibkr_connection ALIAS umicom_ibkr_connection)
set_target_properties(umicom_ibkr_connection PROPERTIES EXPORT_NAME ibkr_connection)
target_include_directories(umicom_ibkr_connection PUBLIC
    $<BUILD_INTERFACE:${_umi_ibkr_root}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_ibkr_connection PUBLIC Umicom::trading)
# New-file output uses the canonical platform handle owner.
target_link_libraries(umicom_ibkr_connection PRIVATE Umicom::platform)
if(WIN32)
    target_link_libraries(umicom_ibkr_connection PRIVATE ws2_32)
endif()
umicom_ibkr_target(umicom_ibkr_connection)
install(TARGETS umicom_ibkr_connection EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(FILES "${_umi_ibkr_root}/include/umicom/broker_connectivity/observation_export.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
# The additional review contract is installed alongside the existing broker API.
install(FILES "${_umi_ibkr_root}/include/umicom/broker_connectivity/position_review.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
install(FILES "${_umi_ibkr_root}/include/umicom/broker_connectivity/connection.h"
    "${_umi_ibkr_root}/include/umicom/broker_connectivity/quotes.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
add_executable(umicom-broker-connect "${_umi_ibkr_root}/examples/ibkr_connection/main.c")
add_executable(umicom-broker-profile-example "${_umi_ibkr_root}/examples/ibkr_connection/lesson.c")
foreach(_target IN ITEMS umicom-broker-connect umicom-broker-profile-example)
    target_link_libraries(${_target} PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(${_target})
    install(TARGETS ${_target} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
endforeach()
if(BUILD_TESTING)
    # Deferred component completion may add targets and tests, but may not
    # create CMake subdirectories. Keep the former registration for review;
    # the same test definitions now use their own absolute source paths.
    if(FALSE)
    add_subdirectory("${_umi_ibkr_root}/tests/ibkr_connection" "${CMAKE_CURRENT_BINARY_DIR}/ibkr-connection-tests")
    endif()
    include("${_umi_ibkr_root}/tests/ibkr_connection/CMakeLists.txt")
endif()
install(FILES "${_umi_ibkr_root}/docs/learning/paper-live-connections.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
unset(_umi_ibkr_root)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/export-broker-observations.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)

# Portable fill intent is available to every client of the existing owners.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/fill_policy.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/trading/fill_policy.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/trading/fill_watch.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/trading COMPONENT Framework)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomFullQuantityChecks.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/full-quantity-orders.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)

# Ship the same educational workflow to installed Framework consumers.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/ibkr-contract-inspection.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/docs/learning" COMPONENT Framework)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomPriceIncrementChecks.cmake")

# Execution capture reuses the read-only connection and exact decimal owner.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/execution_observation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/execution_decode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/execution_identity.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/execution_review.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/execution_observation.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/review-broker-executions.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/learning" COMPONENT Learning)

target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/execution_commission.c")

# P&L and symbol discovery share the bounded read-only session owner.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/observation_wire.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/pnl.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/pnl_decode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/symbol_search.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/symbol_decode.c")
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/pnl.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/symbol_search.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

# Market depth owns positional ladders independently from sampled top-of-book quotes.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/market_depth.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/depth_decode.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/market_depth.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/broker-market-observations.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/learning" COMPONENT Learning)

# Broker order recovery shares identity, exact numeric and transport owners.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/order_observation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/order_identity.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/order_store.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/order_decode.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/order_recovery.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/recover-broker-orders.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/learning" COMPONENT Learning)

# Completed-order history retains broker evidence separately from working orders.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/order_numbers.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/completed_orders.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/completed_store.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/completed_layout.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/completed_decode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/completed_review.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/completed_orders.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/review-completed-orders.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/learning" COMPONENT Learning)

# Reports use the canonical CSV document and existing new-file writer.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/completed_report.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/completed_report.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

# Historical requests and chart projection share the portable chart owner.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/historical_query.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/historical_bars.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/historical_decode.c")
# A graphics-free connection remains usable by headless applications.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomIbkrHistoricalChart.cmake")
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/historical_bars.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/historical_chart.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/broker-historical-charts.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/learning" COMPONENT Learning)

# Historical reports reuse the established owned CSV writer.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/historical_report.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/historical_report.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

# Streaming data shares transport, decimal validation and report ownership.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/market_bar_decode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/realtime_bars.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/realtime_decode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/realtime_report.c")
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/realtime_bars.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/realtime_chart.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/realtime_report.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/broker-streaming-charts.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/learning" COMPONENT Learning)

# Scanner and option discovery share transport and never require graphics.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/scanner_query.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/scanner.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/scanner_decode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/option_chain.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/option_chain_decode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/discovery_lifetime.c")
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/scanner.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/option_chain.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

# Resolve one chosen option using the canonical metadata request owner.
target_sources(umicom_ibkr_connection PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/option_contract.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/option_contract.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

# Owned CSV reports can be written without borrowing a broker connection.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/scanner_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/option_chain_report.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/discovery_report.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

# Selected-option consumers need the same contract metadata declaration.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/contract_details.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/broker-discovery.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/learning" COMPONENT Learning)

# Catalogue storage stays in the headless connection, shared by every front end.
target_sources(umicom_ibkr_connection PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/scanner_catalog.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/scanner_catalog_decode.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/scanner_catalog.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
