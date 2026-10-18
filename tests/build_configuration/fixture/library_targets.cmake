# Umicom Framework
# File: tests/build_configuration/fixture/library_targets.cmake
# PURPOSE: Check that model, workspace and asset-library tests coexist.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT

# Imported dependency names keep this a configure-only fixture. The real module
# files still create the libraries, install rules and regression executables.
foreach(_dependency IN ITEMS data native_launcher)
    add_library(Umicom::${_dependency} INTERFACE IMPORTED)
endforeach()
add_library(umicom_creative_workspace STATIC)
add_library(Umicom::creative_workspace ALIAS umicom_creative_workspace)
add_library(umicom_creative_workspace_gtk4 STATIC)
add_library(Umicom::creative_workspace_gtk4 ALIAS umicom_creative_workspace_gtk4)
function(umicom_creative_configure_target target)
    set_target_properties("${target}" PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
endfunction()

# Reproduce the two existing owners using their real source paths. A module
# must not reuse either name, whichever owner is registered first.
macro(add_existing_library_tests)
    add_executable(umicom-workspace-library-test
        "${ROOT}/tests/test_workspace_library.c")
    add_test(NAME framework.workspace_library COMMAND umicom-workspace-library-test)
    add_executable(umicom-creative-library-gtk-test
        "${ROOT}/tests/creative_workspace/test_library_gtk4.c")
    add_test(NAME framework.creative_workspace.gtk.library
        COMMAND umicom-creative-library-gtk-test)
endmacro()
if(NOT FIXTURE_ORDER STREQUAL "core-first" AND NOT FIXTURE_ORDER STREQUAL "modules-first")
    message(FATAL_ERROR "Unknown library include order: ${FIXTURE_ORDER}")
endif()
if(BUILD_TESTING AND FIXTURE_ORDER STREQUAL "core-first")
    add_existing_library_tests()
endif()
include("${ROOT}/cmake/UmicomWorkspaceLibrary.cmake")
include("${ROOT}/cmake/UmicomCreativeLibrary.cmake")
# A second host can request the same modules; their global guards must ensure
# that sources, executable targets and test registrations remain owned once.
include("${ROOT}/cmake/UmicomWorkspaceLibrary.cmake")
include("${ROOT}/cmake/UmicomCreativeLibrary.cmake")
if(BUILD_TESTING AND FIXTURE_ORDER STREQUAL "modules-first")
    add_existing_library_tests()
endif()

function(require_library_source target expected)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Missing library regression executable: ${target}")
    endif()
    get_target_property(_sources "${target}" SOURCES)
    list(LENGTH _sources _count)
    if(NOT _count EQUAL 1)
        message(FATAL_ERROR "${target} no longer owns one source file")
    endif()
    # Modules may spell the same source with cmake/../ or a resolved root.
    # Compare normalized paths so this check detects ownership, not spelling.
    get_filename_component(_actual "${_sources}" ABSOLUTE)
    get_filename_component(_expected "${ROOT}/${expected}" ABSOLUTE)
    if(NOT _actual STREQUAL _expected)
        message(FATAL_ERROR "${target} no longer owns exactly ${expected}")
    endif()
endfunction()

if(BUILD_TESTING)
    require_library_source(umicom-workspace-library-test tests/test_workspace_library.c)
    require_library_source(umicom-workspace-library-native-test tests/workspace_library/test_library.c)
    require_library_source(umicom-creative-library-gtk-test tests/creative_workspace/test_library_gtk4.c)
    require_library_source(umicom-creative-asset-library-gtk-test tests/creative_library/test_gtk4.c)
    # Keep existing CTest names so selected tests and historical reports remain
    # useful while executable names describe the separate implementations.
    foreach(_case IN ITEMS framework.workspace_library framework.workspace_library.files
            framework.workspace_library.controls framework.creative_workspace.gtk.library)
        if(NOT TEST "${_case}")
            message(FATAL_ERROR "An existing library test disappeared: ${_case}")
        endif()
    endforeach()
    foreach(_case IN ITEMS import duplicate reorder remove-undo save-reopen corrupt-retains
            cancel-import cancel-open busy open-approval approval-invalidation export
            existing-export editor-copy independent retained-control close-busy title)
        if(NOT TEST "framework.creative_library.gtk.${_case}")
            message(FATAL_ERROR "Missing asset-library workflow: ${_case}")
        endif()
    endforeach()
    foreach(_target IN ITEMS umicom-workspace-library-native-test umicom-creative-asset-library-gtk-test)
        foreach(_policy IN ITEMS WARNINGS VALIDATION)
            get_target_property(_present "${_target}" "FIXTURE_${_policy}")
            if(NOT _present)
                message(FATAL_ERROR "${_target} lost the ${_policy} policy")
            endif()
        endforeach()
    endforeach()
else()
    # Production consumers must still receive the libraries without test tools.
    foreach(_target IN ITEMS umicom-workspace-library-test umicom-workspace-library-native-test
            umicom-creative-library-gtk-test umicom-creative-asset-library-gtk-test)
        if(TARGET "${_target}")
            message(FATAL_ERROR "Disabled testing created ${_target}")
        endif()
    endforeach()
endif()

# An independently included module must load SDK registration itself and join
# the aggregate exactly once. This also holds when ordinary tests are disabled.
if(NOT TARGET umicom-sdk-libraries)
    message(FATAL_ERROR "Independent component did not provide the SDK aggregate")
endif()
get_target_property(_sdk_dependencies umicom-sdk-libraries MANUALLY_ADDED_DEPENDENCIES)
foreach(_sdk_owner IN ITEMS umicom_workspace_library_gtk4)
    get_target_property(_sdk_registered "${_sdk_owner}" UMICOM_SDK_INSTALL_REGISTERED)
    if(NOT _sdk_registered OR NOT _sdk_owner IN_LIST _sdk_dependencies)
        message(FATAL_ERROR "Independent component is absent from the SDK: ${_sdk_owner}")
    endif()
endforeach()
