# Umicom Framework
# File: tests/media_generation/comfy_node_pages_fixture.cmake
#
# PURPOSE:
#   Prepare isolated captured catalogues while reusing the canonical saved-connection fixture.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include("${CMAKE_CURRENT_LIST_DIR}/comfy_inspection_fixture.cmake")
function(umicom_comfy_node_pages_fixture root mode)
    umicom_comfy_inspection_fixture("${root}" "${mode}")
    set(content [==[{"Z":{"display_name":"Zulu","category":"image/edit","input":{"required":{"text":["STRING",{"default":"PRIVATE_DEFAULT"}]}},"output":["IMAGE"],"output_node":true,"private":"PRIVATE_EXTENSION"},"A":{"display_name":"Alpha","category":"image","input":{},"output":[],"deprecated":false},"B":{"display_name":"Beta","category":"image2","input":{},"output":[],"experimental":true,"api_node":false},"C":{"display_name":"C\"\n東京","category":"text","input":{},"output":[],"output_node":false}}]==])
    if(mode STREQUAL "empty")
        set(content "{}")
    elseif(mode STREQUAL "empty-file")
        set(content "")
    endif()
    if(NOT mode MATCHES "^(malformed-catalogue|catalogue-limit)$")
        file(WRITE "${INSPECTION_NODES}" "${content}")
    endif()
    set(input "${INSPECTION_NODES}")
    get_filename_component(folder "${INSPECTION_NODES}" DIRECTORY)
    if(mode STREQUAL "missing-file")
        set(input "${folder}/absent-child/nodes.json")
    elseif(mode STREQUAL "directory")
        set(input "${folder}")
    endif()
    set(NODE_DOCUMENT "${INSPECTION_CONNECTION}" PARENT_SCOPE)
    set(NODE_INPUT "${input}" PARENT_SCOPE)
    set(NODE_DATA_FILE "${INSPECTION_NODES}" PARENT_SCOPE)
endfunction()
