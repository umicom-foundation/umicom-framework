# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Production and focused qualification share this exact source definition.
include_guard(GLOBAL)
include(GNUInstallDirs)
function(umicom_add_record_chain_sources target)
    get_filename_component(_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    target_sources("${target}" PRIVATE
        "${_root}/src/record_chain/chain.c"
        "${_root}/src/record_chain/archive.c")
    target_include_directories("${target}" PUBLIC
        "$<BUILD_INTERFACE:${_root}/include>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO EXPORT_NAME record_chain)
    target_link_libraries("${target}" PUBLIC Umicom::base)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings("${target}")
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers("${target}")
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs("${target}" FILES "${_root}/src/record_chain/internal.h")
    endif()
endfunction()
