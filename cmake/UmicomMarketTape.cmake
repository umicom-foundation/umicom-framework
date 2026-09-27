# Umicom Foundation | Sammy Hegab | MIT
# Shared tape semantics. The canonical trading vocabulary is not redefined.
include_guard(GLOBAL)
include(GNUInstallDirs)
if(NOT TARGET Umicom::trading OR NOT TARGET Umicom::platform)
    message(FATAL_ERROR "Market tape requires canonical trading and platform targets")
endif()
set(_umi_tape_root "${CMAKE_CURRENT_LIST_DIR}/..")
function(umicom_market_tape_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()
add_library(umicom_market_tape STATIC
    "${_umi_tape_root}/src/market_tape/tape.c"
    "${_umi_tape_root}/src/market_tape/practice.c")
add_library(Umicom::market_tape ALIAS umicom_market_tape)
set_target_properties(umicom_market_tape PROPERTIES EXPORT_NAME market_tape)
target_link_libraries(umicom_market_tape PUBLIC Umicom::trading Umicom::platform)
if(NOT WIN32)
    target_link_libraries(umicom_market_tape PRIVATE m)
endif()
target_include_directories(umicom_market_tape PUBLIC
    $<BUILD_INTERFACE:${_umi_tape_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
umicom_market_tape_target(umicom_market_tape)
install(TARGETS umicom_market_tape EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(FILES "${_umi_tape_root}/include/umicom/trading/market_tape.h"
    "${_umi_tape_root}/include/umicom/trading/market_tape_practice.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/trading COMPONENT Framework)
add_executable(umicom-market-tape "${_umi_tape_root}/examples/market_tape/main.c")
add_executable(umicom-market-tape-example "${_umi_tape_root}/examples/market_tape/lesson.c")
foreach(_tool IN ITEMS umicom-market-tape umicom-market-tape-example)
    target_link_libraries(${_tool} PRIVATE Umicom::market_tape)
    umicom_market_tape_target(${_tool})
    install(TARGETS ${_tool} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
endforeach()
if(BUILD_TESTING)
    add_subdirectory("${_umi_tape_root}/tests/market_tape" "${CMAKE_CURRENT_BINARY_DIR}/market-tape-tests")
endif()
install(FILES "${_umi_tape_root}/docs/learning/market-tape.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
unset(_umi_tape_root)
