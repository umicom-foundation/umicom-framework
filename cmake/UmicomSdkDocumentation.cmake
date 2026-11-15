# Umicom Framework
# File: cmake/UmicomSdkDocumentation.cmake
#
# PURPOSE:
# Install a source-compatible offline SDK learning and guide tree with approved branding.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)

# Documentation is useful without a compiler or library installation. Keep the
# canonical tree identical in all components so relative artwork and guide links
# retain their source-tree meaning. Existing legacy destinations remain separate.
function(umicom_install_sdk_documentation source_root)
    if(NOT ARGC EQUAL 1 OR NOT IS_ABSOLUTE "${source_root}" OR
       NOT IS_DIRECTORY "${source_root}" OR
       NOT DEFINED CMAKE_INSTALL_DATADIR OR CMAKE_INSTALL_DATADIR STREQUAL "")
        message(FATAL_ERROR "SDK documentation requires an absolute source root and an install data directory")
    endif()
    set(_root_files
        docs/DEVELOPMENT_WORKFLOW.html
        docs/SELECTIVE_SDK_INSTALLATION.html
        docs/KANBAN_PLANNING.html
        docs/windows-installation.html
        docs/WRITING_DOCUMENTATION.md)
    set(_brand_files umicom-logo.svg umicom-logo-on-dark.svg
        umicom-icon.svg umicom-icon-on-dark.svg umicom.ico README.md)
    foreach(_tree docs/guides docs/learning)
        if(NOT IS_DIRECTORY "${source_root}/${_tree}")
            message(FATAL_ERROR "SDK documentation input missing: ${_tree}")
        endif()
    endforeach()
    set(_documents)
    foreach(_file IN LISTS _root_files)
        if(NOT EXISTS "${source_root}/${_file}" OR IS_DIRECTORY "${source_root}/${_file}")
            message(FATAL_ERROR "SDK documentation input missing: ${_file}")
        endif()
        list(APPEND _documents "${source_root}/${_file}")
    endforeach()
    set(_branding)
    foreach(_file IN LISTS _brand_files)
        if(NOT EXISTS "${source_root}/resources/brand/${_file}" OR
           IS_DIRECTORY "${source_root}/resources/brand/${_file}")
            message(FATAL_ERROR "SDK documentation brand input missing: ${_file}")
        endif()
        list(APPEND _branding "${source_root}/resources/brand/${_file}")
    endforeach()

    set(_docs "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs")
    set(_brand "${CMAKE_INSTALL_DATADIR}/umicom/framework/resources/brand")
    foreach(_component Framework FrameworkSDK FrameworkDocumentation)
        set(_selection EXCLUDE_FROM_ALL)
        if(_component STREQUAL "Framework")
            set(_selection)
        endif()
        install(DIRECTORY "${source_root}/docs/guides/"
            DESTINATION "${_docs}/guides" COMPONENT "${_component}" ${_selection})
        install(DIRECTORY "${source_root}/docs/learning/"
            DESTINATION "${_docs}/learning" COMPONENT "${_component}" ${_selection})
        install(FILES ${_documents}
            DESTINATION "${_docs}" COMPONENT "${_component}" ${_selection})
        # Framework-only installation cannot rely on an unscoped resource rule.
        # Reuse identical approved masters; do not regenerate or redraw artwork.
        install(FILES ${_branding}
            DESTINATION "${_brand}" COMPONENT "${_component}" ${_selection})
    endforeach()
endfunction()
