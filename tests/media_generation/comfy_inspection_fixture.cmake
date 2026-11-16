# Umicom Framework
# File: tests/media_generation/comfy_inspection_fixture.cmake
#
# PURPOSE:
#   Prepare isolated synthetic files for native offline inspection regression checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

# Test-owned files are overwritten only inside the caller's separate binary
# directory. No provider, source fixture, vault or user workspace is modified.
function(umicom_comfy_inspection_fixture root mode)
    if(NOT IS_ABSOLUTE "${root}")
        message(FATAL_ERROR "Inspection fixture directory must be absolute")
    endif()
    if(mode STREQUAL "unicode-path")
        string(APPEND root "/équipe-東京")
    endif()
    file(MAKE_DIRECTORY "${root}")
    set(connection "${root}/connection.umiconnection")
    set(workflow "${root}/workflow.json")
    set(nodes "${root}/object-info.json")
    file(READ "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../examples/comfy_inspection/connection.umiconnection" document)
    set(flow [=[{"1":{"class_type":"Text","inputs":{"text":"PRIVATE_SENTINEL_DO_NOT_EMIT"}}}]=])
    set(catalogue [=[{"Text":{"input":{"required":{"text":["STRING"]}},"output":["STRING"],"output_node":false}}]=])
    if(mode STREQUAL "missing")
        set(flow [=[{"1":{"class_type":"Text","inputs":{}}}]=])
    elseif(mode STREQUAL "missing-class")
        set(catalogue "{}")
    elseif(mode STREQUAL "disabled")
        string(REPLACE "\n30000\n1\n" "\n30000\n0\n" document "${document}")
    elseif(mode STREQUAL "malformed-document")
        set(document "not-a-settings-document")
    elseif(mode STREQUAL "malformed-workflow")
        set(flow "{")
    elseif(mode STREQUAL "malformed-catalogue")
        set(catalogue "{")
    elseif(mode STREQUAL "document-limit")
        string(REPEAT " " 8192 document)
    elseif(mode STREQUAL "workflow-limit")
        string(REPEAT " " 1048577 flow)
    elseif(mode STREQUAL "catalogue-limit")
        string(REPEAT " " 4194305 catalogue)
    endif()
    file(WRITE "${connection}" "${document}")
    file(WRITE "${workflow}" "${flow}")
    file(WRITE "${nodes}" "${catalogue}")
    if(mode STREQUAL "missing-file")
        # This reserved child directory is never created by the fixture.
        set(workflow "${root}/absent-child/workflow.json")
    elseif(mode STREQUAL "directory")
        set(workflow "${root}")
    endif()
    set(INSPECTION_CONNECTION "${connection}" PARENT_SCOPE)
    set(INSPECTION_WORKFLOW "${workflow}" PARENT_SCOPE)
    set(INSPECTION_NODES "${nodes}" PARENT_SCOPE)
endfunction()
