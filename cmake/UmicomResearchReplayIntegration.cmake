# Umicom Foundation | Sammy Hegab | MIT
# Wait for the authoritative strategy/trading and native hashing targets.
# Do not construct substitute dependencies in the complete application build.
include_guard(GLOBAL)
function(umicom_research_replay_complete_integration)
    if(TARGET Umicom::trading_ui AND TARGET Umicom::native_launcher AND TARGET Umicom::trading)
        include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomResearchReplay.cmake")
    endif()
endfunction()
cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL umicom_research_replay_complete_integration)
