# Umicom Foundation | Sammy Hegab | MIT
# Compose only after canonical trading targets exist. Minimal unrelated hosts
# retain their prior dependency set rather than gaining a substitute trading SDK.
include_guard(GLOBAL)
function(umicom_market_tape_complete_integration)
    if(NOT TARGET Umicom::trading OR NOT TARGET Umicom::platform)
        return()
    endif()
    include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomMarketTape.cmake")
    if(TARGET umicom_ui_gtk4)
        include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomMarketTapeGtk4.cmake")
        if(TARGET Umicom::market_tape_gtk4)
            target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::market_tape_gtk4)
        endif()
    endif()
endfunction()
cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL umicom_market_tape_complete_integration)
