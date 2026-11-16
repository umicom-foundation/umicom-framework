# Umicom Framework
# File: tests/media_generation/comfy_queue_fixture.cmake
#
# PURPOSE:
#   Reuse canonical settings fixtures and create isolated captured queue inputs for native consumers.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include("${CMAKE_CURRENT_LIST_DIR}/comfy_inspection_fixture.cmake")
function(umicom_comfy_queue_fixture root mode)
    umicom_comfy_inspection_fixture("${root}" "${mode}")
    get_filename_component(queue_root "${INSPECTION_CONNECTION}" DIRECTORY)
    set(path "${queue_root}/queue.json")
    set(content [==[{"queue_running":[[-0,"run\"\n東京",{"text":"PRIVATE_SENTINEL"},{},["PRIVATE_OUTPUT"]]],"queue_pending":[[9007199254740993,"later",{},{"hidden":"PRIVATE_EXTRA"},[]],[2.5E+2,"earlier",{},{},[]]]}]==])
    if(mode STREQUAL "empty")
        set(content [=[{"queue_running":[],"queue_pending":[]}]=])
    elseif(mode STREQUAL "malformed-queue")
        set(content "{")
    elseif(mode STREQUAL "queue-limit")
        string(REPEAT " " 4194305 content)
    endif()
    file(WRITE "${path}" "${content}")
    set(input "${path}")
    set(document "${INSPECTION_CONNECTION}")
    if(mode STREQUAL "missing-file")
        set(input "${queue_root}/absent-child/queue.json")
    elseif(mode STREQUAL "directory")
        set(input "${queue_root}")
    elseif(mode STREQUAL "relative-before-read")
        set(document "${queue_root}/absent-child/settings.umiconnection")
    endif()
    set(QUEUE_DOCUMENT "${document}" PARENT_SCOPE)
    set(QUEUE_INPUT "${input}" PARENT_SCOPE)
    set(QUEUE_DOCUMENT_FILE "${INSPECTION_CONNECTION}" PARENT_SCOPE)
    set(QUEUE_DATA_FILE "${path}" PARENT_SCOPE)
endfunction()
