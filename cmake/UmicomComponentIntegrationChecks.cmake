#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Configuration regression tests exercise the real component integration calls.
# No alternative implementation of a trading service is introduced here.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(BUILD_TESTING)
    include("${CMAKE_CURRENT_LIST_DIR}/../tests/component_integration/CMakeLists.txt")
endif()

# Build the same public-API lesson in a complete product composition. Deferral
# here only creates targets/tests after the three owners exist; no directory
# traversal is performed. A standalone probe may already own this executable.
function(umicom_component_integration_lesson)
    if(TARGET umicom-component-integration-example OR
        NOT TARGET Umicom::market_tape OR NOT TARGET Umicom::research_replay OR
        NOT TARGET Umicom::ibkr_connection)
        return()
    endif()
    add_executable(umicom-component-integration-example
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/component_integration/main.c")
    target_link_libraries(umicom-component-integration-example PRIVATE
        Umicom::market_tape Umicom::research_replay Umicom::ibkr_connection)
    set_target_properties(umicom-component-integration-example PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-component-integration-example)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-component-integration-example)
    endif()
    if(BUILD_TESTING)
        add_test(NAME framework.component_integration.lesson COMMAND umicom-component-integration-example)
    endif()
endfunction()
cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL umicom_component_integration_lesson)
