# Umicom Framework
# File: tests/media_generation/comfy_system_fixture.cmake
#
# PURPOSE:
#   Create isolated captured system reports using the canonical saved-connection fixture.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


include("${CMAKE_CURRENT_LIST_DIR}/comfy_inspection_fixture.cmake")
function(umicom_comfy_system_fixture root mode)
    umicom_comfy_inspection_fixture("${root}" "${mode}")
    get_filename_component(system_root "${INSPECTION_CONNECTION}" DIRECTORY)
    set(path "${system_root}/system.json")
    set(content [==[{"system":{"os":"test\"\n東京","embedded_python":false,"ram_total":9223372036854775807,"ram_free":9007199254740993,"argv":["PRIVATE_ARG"]},"devices":[{"name":"CPU","type":"cpu","index":null},{"name":"GPU","type":"future","index":0,"vram_total":8,"vram_free":9,"torch_vram_total":4,"torch_vram_free":1,"extra":"PRIVATE_DEVICE"}],"private":"PRIVATE_EXTENSION"}]==])
    if(mode STREQUAL "empty")
        set(content [=[{"system":{},"devices":[]}]=])
    elseif(mode STREQUAL "malformed-system")
        set(content "{")
    elseif(mode STREQUAL "system-limit")
        string(REPEAT " " 1048577 content)
    elseif(mode STREQUAL "empty-file")
        set(content "")
    endif()
    file(WRITE "${path}" "${content}")
    set(input "${path}")
    set(document "${INSPECTION_CONNECTION}")
    if(mode STREQUAL "missing-file")
        set(input "${system_root}/absent-child/system.json")
    elseif(mode STREQUAL "directory")
        set(input "${system_root}")
    elseif(mode STREQUAL "relative-before-read")
        set(document "${system_root}/absent-child/settings.umiconnection")
    endif()
    set(SYSTEM_DOCUMENT "${document}" PARENT_SCOPE)
    set(SYSTEM_INPUT "${input}" PARENT_SCOPE)
    set(SYSTEM_DOCUMENT_FILE "${INSPECTION_CONNECTION}" PARENT_SCOPE)
    set(SYSTEM_DATA_FILE "${path}" PARENT_SCOPE)
endfunction()
