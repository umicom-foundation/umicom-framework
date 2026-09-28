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
    "${_umi_ibkr_root}/src/ibkr_connection/network.c")
add_library(Umicom::ibkr_connection ALIAS umicom_ibkr_connection)
set_target_properties(umicom_ibkr_connection PROPERTIES EXPORT_NAME ibkr_connection)
target_include_directories(umicom_ibkr_connection PUBLIC
    $<BUILD_INTERFACE:${_umi_ibkr_root}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_ibkr_connection PUBLIC Umicom::trading)
if(WIN32)
    target_link_libraries(umicom_ibkr_connection PRIVATE ws2_32)
endif()
umicom_ibkr_target(umicom_ibkr_connection)
install(TARGETS umicom_ibkr_connection EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(FILES "${_umi_ibkr_root}/include/umicom/broker_connectivity/connection.h"
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
