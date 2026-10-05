# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Reuse the same complete C example in a standalone SDK build and Web Studio.
# The product selects a target name; it does not copy sockets or request logic.
function(umicom_add_local_web_service target)
    if(NOT TARGET Umicom::web OR TARGET "${target}")
        message(FATAL_ERROR "Local web service needs Umicom::web and an unused target name")
    endif()
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/web/local_service.c")
    target_link_libraries("${target}" PRIVATE Umicom::web)
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    install(TARGETS "${target}" RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
endfunction()
if(UMICOM_BUILD_LEARNING_EXAMPLES)
    umicom_add_local_web_service(umicom-web-local-service-example)
endif()
