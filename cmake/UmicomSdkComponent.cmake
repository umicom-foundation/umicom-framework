# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# A selective SDK must contain the same libraries referenced by the existing
# export set. Keep full-install rules intact and add opt-in artifact rules.
include_guard(GLOBAL)
include(GNUInstallDirs)

# Register real local libraries after their normal export-set installation.
# This target deliberately excludes application executables and is not in ALL.
# Registration is immediate so optional and deferred composition modules can
# join it without depending on the order of a final directory callback.
function(umicom_register_sdk_libraries)
    if(NOT ARGC GREATER 0)
        message(FATAL_ERROR "Framework SDK registration requires library targets")
    endif()
    foreach(_target IN LISTS ARGN)
        if(NOT TARGET "${_target}")
            message(FATAL_ERROR "Framework SDK target does not exist: ${_target}")
        endif()
        get_target_property(_alias "${_target}" ALIASED_TARGET)
        get_target_property(_imported "${_target}" IMPORTED)
        get_target_property(_type "${_target}" TYPE)
        if(_alias OR _imported)
            message(FATAL_ERROR "Framework SDK requires an owned library: ${_target}")
        endif()
        if(NOT _type MATCHES "^(STATIC|SHARED|MODULE|INTERFACE)_LIBRARY$")
            message(FATAL_ERROR "Framework SDK target is not an installable library: ${_target}")
        endif()
        # Bundled Apple frameworks need separate layout rules; do not silently omit them.
        get_target_property(_bundle "${_target}" FRAMEWORK)
        if(_bundle)
            message(FATAL_ERROR "Framework SDK does not support framework bundles: ${_target}")
        endif()
        get_target_property(_registered "${_target}" UMICOM_SDK_INSTALL_REGISTERED)
        if(_registered)
            continue()
        endif()
        if(NOT TARGET umicom-sdk-libraries)
            add_custom_target(umicom-sdk-libraries)
            set_property(TARGET umicom-sdk-libraries PROPERTY UMICOM_SDK_BUILD_TARGET TRUE)
        else()
            get_target_property(_owned umicom-sdk-libraries UMICOM_SDK_BUILD_TARGET)
            if(NOT _owned)
                message(FATAL_ERROR "Framework SDK build target name is already in use")
            endif()
        endif()
        # The original rule owns EXPORT registration. Re-registering that same
        # target in the export set would produce duplicate imported definitions.
        # Windows shared libraries need both their DLL and their import archive.
        install(TARGETS "${_target}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
                COMPONENT FrameworkSDK EXCLUDE_FROM_ALL
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
                COMPONENT FrameworkSDK EXCLUDE_FROM_ALL
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
                COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
        add_dependencies(umicom-sdk-libraries "${_target}")
        set_property(TARGET "${_target}" PROPERTY UMICOM_SDK_INSTALL_REGISTERED TRUE)
    endforeach()
endfunction()
