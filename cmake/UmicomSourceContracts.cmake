#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSourceContracts.cmake
# PURPOSE: Attach the one native source-contract tool to the existing build graph.
# Author: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
function(umicom_enable_source_contracts)
    if(TARGET umicom-source-contracts)
        return()
    endif()
    # An installed Framework export already owns this native command. Reuse
    # that imported executable rather than requiring SDK users to carry src/.
    if(TARGET Umicom::umicom-source-contracts)
        add_executable(umicom-source-contracts ALIAS Umicom::umicom-source-contracts)
        return()
    endif()
    add_subdirectory("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tools/source_contracts"
        "${CMAKE_BINARY_DIR}/framework-source-contracts")
endfunction()
