# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Reuse the same storage adapter in full builds and isolated qualification.
include_guard(GLOBAL)
include(GNUInstallDirs)
function(umicom_add_kanban_storage_sources target)
    get_filename_component(_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    target_sources("${target}" PRIVATE
        "${_root}/src/kanban/project_binding.c"
        "${_root}/src/kanban/project_binding_archive.c"
        "${_root}/src/kanban/binding_storage.c"
        "${_root}/src/kanban/publication_plan_storage.c"
        "${_root}/src/kanban/completion_storage.c"
        "${_root}/src/kanban/location.c" "${_root}/src/kanban/document.c" "${_root}/src/kanban/document_save.c")
    set_target_properties("${target}" PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO EXPORT_NAME kanban_storage)
    target_include_directories("${target}" PUBLIC
        "$<BUILD_INTERFACE:${_root}/include>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    target_link_libraries("${target}" PUBLIC Umicom::kanban Umicom::platform)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings("${target}")
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers("${target}")
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs("${target}" FILES "${_root}/src/kanban/document_internal.h"
            "${_root}/src/base/value_archive_internal.h")
    endif()
endfunction()
