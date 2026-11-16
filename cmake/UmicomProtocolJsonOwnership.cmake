# Umicom Framework
# File: cmake/UmicomProtocolJsonOwnership.cmake
#
# PURPOSE:
# Share the existing owned JSON and Unicode services through the protocol library.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
if(NOT TARGET umicom_protocol)
    message(FATAL_ERROR "Owned JSON requires the canonical protocol target")
endif()
# These implementations remain at their public compatibility paths. One archive
# owns their symbols, allowing protocol and AI consumers to avoid an IDE dependency.
target_sources(umicom_protocol PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_text.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_tree.c")

# Shared response ownership reuses the existing non-elidable clearing service.
target_link_libraries(umicom_protocol PRIVATE Umicom::security)
