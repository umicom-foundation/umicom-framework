# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Complete Framework and focused qualification compile the same production sources.
include_guard(GLOBAL)
include(GNUInstallDirs)
function(umicom_add_kanban_sources target)
    get_filename_component(_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    target_sources("${target}" PRIVATE
        "${_root}/src/kanban/board.c"
        "${_root}/src/kanban/validation.c"
        "${_root}/src/kanban/transaction.c"
        "${_root}/src/kanban/view.c"
        "${_root}/src/kanban/archive.c"
        "${_root}/src/kanban/publication.c"
        "${_root}/src/kanban/publication_fields.c"
        "${_root}/src/kanban/publication_plan.c"
        "${_root}/src/kanban/publication_plan_archive.c"
        "${_root}/src/kanban/developers.c"
        "${_root}/src/kanban/card_details.c"
        "${_root}/src/kanban/column_policy.c"
        "${_root}/src/kanban/completion_plan.c"
        "${_root}/src/kanban/completion_validation.c"
        "${_root}/src/kanban/completion_board.c"
        "${_root}/src/kanban/completion_archive.c")
    set_target_properties("${target}" PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO EXPORT_NAME kanban)
    target_include_directories("${target}" PUBLIC
        "$<BUILD_INTERFACE:${_root}/include>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    target_link_libraries("${target}" PUBLIC Umicom::base)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings("${target}")
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers("${target}")
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs("${target}" FILES
            "${_root}/src/kanban/board_internal.h"
            "${_root}/src/kanban/completion_plan_internal.h"
            "${_root}/src/kanban/publication_plan_internal.h"
            "${_root}/src/base/value_archive_internal.h")
    endif()
endfunction()
