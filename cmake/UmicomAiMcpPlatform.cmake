#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiMcpPlatform.cmake
#
# PURPOSE:
#   Extend the existing provider-neutral AI target with MCP client support,
#   MCP-to-tool-registry integration and hybrid retrieval/reranking.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

if(NOT TARGET umicom_ai)
    message(FATAL_ERROR
        "UmicomAiMcpPlatform requires existing umicom_ai")
endif()

# MCP tools are projected into the existing UmiAiToolRegistry. Existing
# UmiAiPolicy checks therefore remain the execution-authorisation boundary.
target_sources(umicom_ai PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/client.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/codec.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/discovery.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/invocation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/json.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/json_array.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/prompt_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/protocol.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/resource_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/server.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/server_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/tool_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/tool_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/mcp/transport.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/reranker.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/hybrid_retrieval.c"
)

if(BUILD_TESTING AND NOT TARGET umicom-ai-mcp-tests)
    add_executable(umicom-ai-mcp-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_array_empty.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_array_malformed.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_array_missing.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_array_nested_object.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_array_two_objects.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_bridge_invoke.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_bridge_policy_denied.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_bridge_register.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_call_before_ready.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_call_tool.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_prompt_add.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_prompt_at.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_prompt_duplicate.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_prompt_find.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_resource_add.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_resource_at.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_resource_duplicate.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_resource_find.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_tool_add.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_tool_at.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_tool_duplicate.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_catalogue_tool_find.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_client_discover.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_client_init.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_client_register_tools.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_client_start.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_codec_encode_capacity.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_codec_encode_initialize.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_codec_encode_prompt.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_codec_encode_resource.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_codec_encode_tool.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_codec_initialize.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_codec_prompt.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_codec_resource.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_codec_tool.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_codec_tool_missing_name.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_discover_before_ready.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_discover_prompts.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_discover_resources.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_discover_tools.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_get_prompt.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_json_escape.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_json_escape_capacity.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_json_escaped_string.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_json_has_key.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_json_invalid_string.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_json_missing_string.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_json_raw_array.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_json_raw_capacity.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_json_raw_object.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_json_string.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_protocol_initialize.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_protocol_initialized.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_protocol_ping.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_protocol_prompts_get.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_protocol_prompts_list.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_protocol_resources_list.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_protocol_resources_read.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_protocol_tools_call.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_protocol_tools_list.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_read_resource.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_server_init.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_server_invalid_id.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_server_invalid_trust.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_server_registry_add.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_server_registry_at.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_server_registry_duplicate.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_server_registry_find.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_server_registry_remove.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_server_registry_remove_missing.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_server_registry_revision.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_session_capabilities.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_session_disconnect.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_session_init.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_session_initialize.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_session_initialize_twice.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_session_ping.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_session_ping_before_ready.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_mcp/test_session_transport_failure.c"
    )
    target_link_libraries(umicom-ai-mcp-tests PRIVATE
        Umicom::ai)
    umicom_apply_warnings(umicom-ai-mcp-tests)
    umicom_apply_sanitizers(umicom-ai-mcp-tests)
    add_test(
        NAME framework.ai.mcp
        COMMAND umicom-ai-mcp-tests)
endif()

if(BUILD_TESTING AND NOT TARGET umicom-ai-hybrid-retrieval-tests)
    add_executable(umicom-ai-hybrid-retrieval-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_capacity.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_custom_weights.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_invalid.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_lexical_only.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_merge_same_chunk.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_reranker.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_reranker_identity.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_reranker_invalid.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_settings.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_hybrid_retrieval/test_vector_only_candidate.c"
    )
    target_link_libraries(umicom-ai-hybrid-retrieval-tests PRIVATE
        Umicom::ai)
    umicom_apply_warnings(umicom-ai-hybrid-retrieval-tests)
    umicom_apply_sanitizers(umicom-ai-hybrid-retrieval-tests)
    add_test(
        NAME framework.ai.hybrid_retrieval
        COMMAND umicom-ai-hybrid-retrieval-tests)
endif()
