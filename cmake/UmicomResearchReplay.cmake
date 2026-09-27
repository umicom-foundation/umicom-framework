# Umicom Foundation | Sammy Hegab | MIT
include_guard(GLOBAL)
include(GNUInstallDirs)
foreach(_dep IN ITEMS base trading trading_ui native_launcher)
    if(NOT TARGET Umicom::${_dep})
        message(FATAL_ERROR "Research replay requires canonical Umicom::${_dep}")
    endif()
endforeach()
set(_umi_research_root "${CMAKE_CURRENT_LIST_DIR}/..")
function(umicom_research_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
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
add_library(umicom_research_replay STATIC
    "${_umi_research_root}/src/research_replay/replay.c"
    "${_umi_research_root}/src/research_replay/csv.c")
add_library(Umicom::research_replay ALIAS umicom_research_replay)
set_target_properties(umicom_research_replay PROPERTIES EXPORT_NAME research_replay)
target_include_directories(umicom_research_replay PUBLIC
    $<BUILD_INTERFACE:${_umi_research_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_research_replay PUBLIC Umicom::trading_ui Umicom::trading
    PRIVATE Umicom::native_launcher)
if(NOT WIN32)
    target_link_libraries(umicom_research_replay PRIVATE m)
endif()
umicom_research_target(umicom_research_replay)
install(TARGETS umicom_research_replay EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(FILES
    "${_umi_research_root}/include/umicom/strategy_research/research_replay.h"
    "${_umi_research_root}/include/umicom/strategy_research/research_csv.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/strategy_research COMPONENT Framework)
add_executable(umicom-research-replay
    "${_umi_research_root}/examples/research_replay/main.c"
    "${_umi_research_root}/examples/research_replay/strategy.c")
target_link_libraries(umicom-research-replay PRIVATE Umicom::research_replay)
umicom_research_target(umicom-research-replay)
install(TARGETS umicom-research-replay RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
install(FILES "${_umi_research_root}/docs/learning/research-replay.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    add_subdirectory("${_umi_research_root}/tests/research_replay" "${CMAKE_CURRENT_BINARY_DIR}/research-replay-tests")
endif()
unset(_umi_research_root)
