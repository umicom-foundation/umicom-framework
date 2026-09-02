#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationBranding.cmake
#
# PURPOSE:
#   Apply the shared Umicom icon, Windows identity and runtime brand resources
#   to any application target through one reusable CMake function.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
include(CMakeParseArguments)

# Attach canonical brand files to one executable without copying product logic
# into the application repository. Product names stay native text at runtime.
function(umicom_apply_application_branding)
    # Installation folders are resolved only when a real project invokes the
    # helper, after CMake knows the target platform and architecture.
    include(GNUInstallDirs)
    set(options WINDOWS_GUI DESKTOP_ENTRY RASTER_FALLBACKS)
    set(one_value_args
        TARGET
        PRODUCT_NAME
        INTERNAL_NAME
        APPLICATION_ID
        VERSION_MAJOR
        VERSION_MINOR
        VERSION_PATCH
        RESOURCE_ROOT)
    cmake_parse_arguments(UMICOM_BRAND
        "${options}" "${one_value_args}" "" ${ARGN})

    # Apply this branch only when its contract condition is satisfied.
    if(NOT UMICOM_BRAND_TARGET OR NOT TARGET "${UMICOM_BRAND_TARGET}")
        message(FATAL_ERROR
            "umicom_apply_application_branding requires an executable TARGET")
    endif()
    # Apply this branch only when its contract condition is satisfied.
    if(NOT UMICOM_BRAND_PRODUCT_NAME OR NOT UMICOM_BRAND_INTERNAL_NAME)
        message(FATAL_ERROR
            "Umicom application branding requires PRODUCT_NAME and INTERNAL_NAME")
    endif()
    # Use the stable identifier comparison to choose the matching record or policy.
    if(NOT UMICOM_BRAND_APPLICATION_ID)
        set(UMICOM_BRAND_APPLICATION_ID "${UMICOM_BRAND_INTERNAL_NAME}")
    endif()

    # Source builds pass the Framework resource root. Installed consumers use
    # the resource root exported by the Framework package configuration.
    if(UMICOM_BRAND_RESOURCE_ROOT)
        set(_umicom_brand_root "${UMICOM_BRAND_RESOURCE_ROOT}")
    # Apply this branch only when its contract condition is satisfied.
    elseif(UMICOM_FRAMEWORK_RESOURCE_ROOT)
        set(_umicom_brand_root "${UMICOM_FRAMEWORK_RESOURCE_ROOT}")
    # Use this fallback path when the earlier condition does not apply.
    else()
        get_filename_component(_umicom_brand_root
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../resources" ABSOLUTE)
    endif()
    # Visit each bounded item once so every record receives the same rule.
    foreach(_umicom_brand_file
            "brand/umicom-icon.svg"
            "brand/umicom-icon-on-dark.svg"
            "brand/umicom-logo.svg"
            "brand/umicom-logo-on-dark.svg")
        # Apply this branch only when its contract condition is satisfied.
        if(NOT EXISTS "${_umicom_brand_root}/${_umicom_brand_file}")
            message(FATAL_ERROR
                "Required Umicom brand resource is missing: "
                "${_umicom_brand_root}/${_umicom_brand_file}")
        endif()
    endforeach()

    # Platform packaging formats are checked only by the platform that uses
    # them. This lets a web or Linux build consume the SVG masters without
    # depending on a Windows-only compatibility container.
    if(WIN32)
        # Visit each bounded item once so every record receives the same rule.
        foreach(_umicom_windows_brand_file
                "brand/umicom.ico"
                "windows/umicom-application.rc.in")
            # Apply this branch only when its contract condition is satisfied.
            if(NOT EXISTS
                    "${_umicom_brand_root}/${_umicom_windows_brand_file}")
                message(FATAL_ERROR
                    "Required Windows brand resource is missing: "
                    "${_umicom_brand_root}/${_umicom_windows_brand_file}")
            endif()
        endforeach()
    endif()
    # Apply this branch only when its contract condition is satisfied.
    if(UNIX AND NOT APPLE AND UMICOM_BRAND_DESKTOP_ENTRY AND
            NOT EXISTS
                "${_umicom_brand_root}/linux/umicom-application.desktop.in")
        message(FATAL_ERROR
            "Required Linux desktop-entry template is missing: "
            "${_umicom_brand_root}/linux/umicom-application.desktop.in")
    endif()

    # Apply this branch only when its contract condition is satisfied.
    if(NOT DEFINED UMICOM_BRAND_VERSION_MAJOR OR
            UMICOM_BRAND_VERSION_MAJOR STREQUAL "")
        set(UMICOM_BRAND_VERSION_MAJOR 0)
    endif()
    # Apply this branch only when its contract condition is satisfied.
    if(NOT DEFINED UMICOM_BRAND_VERSION_MINOR OR
            UMICOM_BRAND_VERSION_MINOR STREQUAL "")
        set(UMICOM_BRAND_VERSION_MINOR 1)
    endif()
    # Apply this branch only when its contract condition is satisfied.
    if(NOT DEFINED UMICOM_BRAND_VERSION_PATCH OR
            UMICOM_BRAND_VERSION_PATCH STREQUAL "")
        set(UMICOM_BRAND_VERSION_PATCH 0)
    endif()

    # Windows resources use commas while human-readable metadata uses dots.
    string(CONCAT UMICOM_WINDOWS_FILE_VERSION
        "${UMICOM_BRAND_VERSION_MAJOR},${UMICOM_BRAND_VERSION_MINOR},"
        "${UMICOM_BRAND_VERSION_PATCH},0")
    set(UMICOM_WINDOWS_PRODUCT_VERSION "${UMICOM_WINDOWS_FILE_VERSION}")
    string(CONCAT UMICOM_WINDOWS_FILE_VERSION_TEXT
        "${UMICOM_BRAND_VERSION_MAJOR}.${UMICOM_BRAND_VERSION_MINOR}."
        "${UMICOM_BRAND_VERSION_PATCH}.0")
    set(UMICOM_WINDOWS_PRODUCT_VERSION_TEXT
        "${UMICOM_WINDOWS_FILE_VERSION_TEXT}")
    set(UMICOM_WINDOWS_FILE_DESCRIPTION "${UMICOM_BRAND_PRODUCT_NAME}")
    set(UMICOM_WINDOWS_INTERNAL_NAME "${UMICOM_BRAND_INTERNAL_NAME}")
    set(UMICOM_WINDOWS_ORIGINAL_FILENAME "${UMICOM_BRAND_INTERNAL_NAME}.exe")
    set(UMICOM_WINDOWS_PRODUCT_NAME "${UMICOM_BRAND_PRODUCT_NAME}")
    file(TO_CMAKE_PATH
        "${_umicom_brand_root}/brand/umicom.ico"
        UMICOM_WINDOWS_ICON_PATH)
    # A Windows RCDATA entry can hold the canonical vector bytes unchanged.
    # The application may render this resource with an SVG-capable frontend;
    # the Windows shell continues to use the required multi-size ICO above.
    file(TO_CMAKE_PATH
        "${_umicom_brand_root}/brand/umicom-icon-on-dark.svg"
        UMICOM_WINDOWS_BRAND_MARK_PATH)

    # Apply this branch only when its contract condition is satisfied.
    if(WIN32)
        set(_umicom_brand_binary_dir
            "${CMAKE_CURRENT_BINARY_DIR}/umicom-branding/${UMICOM_BRAND_TARGET}")
        file(MAKE_DIRECTORY "${_umicom_brand_binary_dir}")
        configure_file(
            "${_umicom_brand_root}/windows/umicom-application.rc.in"
            "${_umicom_brand_binary_dir}/${UMICOM_BRAND_INTERNAL_NAME}.rc"
            @ONLY
            NEWLINE_STYLE WIN32)
        target_sources("${UMICOM_BRAND_TARGET}" PRIVATE
            "${_umicom_brand_binary_dir}/${UMICOM_BRAND_INTERNAL_NAME}.rc")
        # Apply this branch only when its contract condition is satisfied.
        if(UMICOM_BRAND_WINDOWS_GUI)
            set_property(TARGET "${UMICOM_BRAND_TARGET}"
                PROPERTY WIN32_EXECUTABLE TRUE)
        endif()
    endif()

    # Keep both contrast-aware vectors beside every GUI executable so runtime
    # title bars and splash screens remain sharp at every display scale.
    add_custom_command(TARGET "${UMICOM_BRAND_TARGET}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory
            "$<TARGET_FILE_DIR:${UMICOM_BRAND_TARGET}>/branding"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${_umicom_brand_root}/brand/umicom-icon.svg"
            "$<TARGET_FILE_DIR:${UMICOM_BRAND_TARGET}>/branding/umicom-icon.svg"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${_umicom_brand_root}/brand/umicom-icon-on-dark.svg"
            "$<TARGET_FILE_DIR:${UMICOM_BRAND_TARGET}>/branding/umicom-icon-on-dark.svg"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${_umicom_brand_root}/brand/umicom-logo.svg"
            "$<TARGET_FILE_DIR:${UMICOM_BRAND_TARGET}>/branding/umicom-logo.svg"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${_umicom_brand_root}/brand/umicom-logo-on-dark.svg"
            "$<TARGET_FILE_DIR:${UMICOM_BRAND_TARGET}>/branding/umicom-logo-on-dark.svg"
        COMMENT "Applying shared Umicom identity to ${UMICOM_BRAND_PRODUCT_NAME}")

    # A product can explicitly request raster fallbacks for an older frontend.
    # They are never the master artwork and are not required by modern Umicom
    # applications. Keeping this opt-in path preserves compatibility without
    # making every desktop, web or mobile package carry duplicate artwork.
    if(UMICOM_BRAND_RASTER_FALLBACKS)
        # Visit each bounded item once so every record receives the same rule.
        foreach(_umicom_raster_file "umicom-icon.png" "umicom-logo.png")
            # Apply this branch only when its contract condition is satisfied.
            if(NOT EXISTS
                    "${_umicom_brand_root}/brand/${_umicom_raster_file}")
                message(FATAL_ERROR
                    "Requested Umicom raster fallback is missing: "
                    "${_umicom_brand_root}/brand/${_umicom_raster_file}")
            endif()
            add_custom_command(TARGET "${UMICOM_BRAND_TARGET}" POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${_umicom_brand_root}/brand/${_umicom_raster_file}"
                    "$<TARGET_FILE_DIR:${UMICOM_BRAND_TARGET}>/branding/${_umicom_raster_file}"
                COMMENT
                    "Copying optional raster fallback for ${UMICOM_BRAND_PRODUCT_NAME}")
        endforeach()
    endif()

    # Installed applications share the canonical scalable icon. A generated
    # desktop entry supplies the product's native text and executable name.
    install(FILES
        "${_umicom_brand_root}/brand/umicom-icon.svg"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
        RENAME "umicom.svg")
    install(FILES
        "${_umicom_brand_root}/brand/umicom-icon.svg"
        "${_umicom_brand_root}/brand/umicom-icon-on-dark.svg"
        "${_umicom_brand_root}/brand/umicom-logo.svg"
        "${_umicom_brand_root}/brand/umicom-logo-on-dark.svg"
        DESTINATION "${CMAKE_INSTALL_BINDIR}/branding")
    # Apply this branch only when its contract condition is satisfied.
    if(UMICOM_BRAND_RASTER_FALLBACKS)
        install(FILES
            "${_umicom_brand_root}/brand/umicom-icon.png"
            "${_umicom_brand_root}/brand/umicom-logo.png"
            DESTINATION "${CMAKE_INSTALL_BINDIR}/branding")
    endif()
    # Apply this branch only when its contract condition is satisfied.
    if(UNIX AND NOT APPLE AND UMICOM_BRAND_DESKTOP_ENTRY)
        set(UMICOM_DESKTOP_APPLICATION_NAME "${UMICOM_BRAND_PRODUCT_NAME}")
        set(UMICOM_DESKTOP_EXECUTABLE_NAME "${UMICOM_BRAND_INTERNAL_NAME}")
        set(UMICOM_DESKTOP_STARTUP_CLASS "${UMICOM_BRAND_APPLICATION_ID}")
        string(CONCAT _umicom_desktop_file
            "${CMAKE_CURRENT_BINARY_DIR}/umicom-branding/"
            "${UMICOM_BRAND_TARGET}/${UMICOM_BRAND_INTERNAL_NAME}.desktop")
        file(MAKE_DIRECTORY
            "${CMAKE_CURRENT_BINARY_DIR}/umicom-branding/${UMICOM_BRAND_TARGET}")
        configure_file(
            "${_umicom_brand_root}/linux/umicom-application.desktop.in"
            "${_umicom_desktop_file}"
            @ONLY
            NEWLINE_STYLE UNIX)
        install(FILES "${_umicom_desktop_file}"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
    endif()
endfunction()
