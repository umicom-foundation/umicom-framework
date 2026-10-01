# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Compose persistence separately from the toolkit-neutral chart model.
include_guard(GLOBAL)
if(NOT TARGET Umicom::native_launcher)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeLauncher.cmake")
endif()
add_library(umicom_chart_checkpoint STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../src/chart/checkpoint_codec.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/chart/checkpoint_store.c")
add_library(Umicom::chart_checkpoint ALIAS umicom_chart_checkpoint)
set_target_properties(umicom_chart_checkpoint PROPERTIES EXPORT_NAME chart_checkpoint
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_chart_checkpoint PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
# Reuse the existing bounded field codec, Data Server and streaming digest.
# Static linking pulls the digest implementation, not launcher process logic.
target_link_libraries(umicom_chart_checkpoint PUBLIC Umicom::chart Umicom::data
    PRIVATE Umicom::workbench_layout_data Umicom::native_launcher)
umicom_apply_warnings(umicom_chart_checkpoint)
umicom_apply_sanitizers(umicom_chart_checkpoint)
install(TARGETS umicom_chart_checkpoint EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)

# Trader's persistence coordinator is toolkit-neutral and can also be hosted by
# Studio. Keeping it separate avoids storage dependencies in chart mathematics.
add_library(umicom_trading_chart_persistence STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../src/trading/chart_persistence.c")
add_library(Umicom::trading_chart_persistence ALIAS umicom_trading_chart_persistence)
set_target_properties(umicom_trading_chart_persistence PROPERTIES EXPORT_NAME trading_chart_persistence
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_trading_chart_persistence PUBLIC Umicom::trading Umicom::chart_checkpoint)
target_include_directories(umicom_trading_chart_persistence PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
umicom_apply_warnings(umicom_trading_chart_persistence)
umicom_apply_sanitizers(umicom_trading_chart_persistence)
install(TARGETS umicom_trading_chart_persistence EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)

# The complete Framework interface includes the new service; smaller consumers
# can continue linking only the chart model or the storage adapter.
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE Umicom::trading_chart_persistence)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomChartCheckpointTests.cmake")
