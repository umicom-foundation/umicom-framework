# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
function(umicom_ibkr_complete_integration)
    if(NOT TARGET Umicom::trading)
        return()
    endif()
    include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomIbkrConnection.cmake")
    if(TARGET umicom_ui_gtk4)
        include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomIbkrConnectionGtk4.cmake")
        if(TARGET umicom-trader AND TARGET Umicom::ibkr_connection_gtk4)
            # Only the Trader entry point acquires this component. Other GTK
            # products keep their existing dependency graph and startup.
            target_link_libraries(umicom-trader PRIVATE Umicom::ibkr_connection_gtk4)
            target_compile_definitions(umicom-trader PRIVATE UMICOM_HAS_IBKR_CONNECTION_GTK4=1)
        endif()
    endif()
endfunction()
cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL umicom_ibkr_complete_integration)
