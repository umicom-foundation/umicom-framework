/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_main.c
 *
 * PURPOSE:
 *   Run focused MCP protocol, transport, discovery, invocation, bridge and policy-integration tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

int test_ai_mcp_array_empty(void);
int test_ai_mcp_array_malformed(void);
int test_ai_mcp_array_missing(void);
int test_ai_mcp_array_nested_object(void);
int test_ai_mcp_array_two_objects(void);
int test_ai_mcp_bridge_invoke(void);
int test_ai_mcp_bridge_policy_denied(void);
int test_ai_mcp_bridge_register(void);
int test_ai_mcp_call_before_ready(void);
int test_ai_mcp_call_tool(void);
int test_ai_mcp_catalogue_prompt_add(void);
int test_ai_mcp_catalogue_prompt_at(void);
int test_ai_mcp_catalogue_prompt_duplicate(void);
int test_ai_mcp_catalogue_prompt_find(void);
int test_ai_mcp_catalogue_resource_add(void);
int test_ai_mcp_catalogue_resource_at(void);
int test_ai_mcp_catalogue_resource_duplicate(void);
int test_ai_mcp_catalogue_resource_find(void);
int test_ai_mcp_catalogue_tool_add(void);
int test_ai_mcp_catalogue_tool_at(void);
int test_ai_mcp_catalogue_tool_duplicate(void);
int test_ai_mcp_catalogue_tool_find(void);
int test_ai_mcp_client_discover(void);
int test_ai_mcp_client_init(void);
int test_ai_mcp_client_register_tools(void);
int test_ai_mcp_client_start(void);
int test_ai_mcp_codec_encode_capacity(void);
int test_ai_mcp_codec_encode_initialize(void);
int test_ai_mcp_codec_encode_prompt(void);
int test_ai_mcp_codec_encode_resource(void);
int test_ai_mcp_codec_encode_tool(void);
int test_ai_mcp_codec_initialize(void);
int test_ai_mcp_codec_prompt(void);
int test_ai_mcp_codec_resource(void);
int test_ai_mcp_codec_tool(void);
int test_ai_mcp_codec_tool_missing_name(void);
int test_ai_mcp_discover_before_ready(void);
int test_ai_mcp_discover_prompts(void);
int test_ai_mcp_discover_resources(void);
int test_ai_mcp_discover_tools(void);
int test_ai_mcp_get_prompt(void);
int test_ai_mcp_json_escape(void);
int test_ai_mcp_json_escape_capacity(void);
int test_ai_mcp_json_escaped_string(void);
int test_ai_mcp_json_has_key(void);
int test_ai_mcp_json_invalid_string(void);
int test_ai_mcp_json_missing_string(void);
int test_ai_mcp_json_raw_array(void);
int test_ai_mcp_json_raw_capacity(void);
int test_ai_mcp_json_raw_object(void);
int test_ai_mcp_json_string(void);
int test_ai_mcp_protocol_initialize(void);
int test_ai_mcp_protocol_initialized(void);
int test_ai_mcp_protocol_ping(void);
int test_ai_mcp_protocol_prompts_get(void);
int test_ai_mcp_protocol_prompts_list(void);
int test_ai_mcp_protocol_resources_list(void);
int test_ai_mcp_protocol_resources_read(void);
int test_ai_mcp_protocol_tools_call(void);
int test_ai_mcp_protocol_tools_list(void);
int test_ai_mcp_read_resource(void);
int test_ai_mcp_server_init(void);
int test_ai_mcp_server_invalid_id(void);
int test_ai_mcp_server_invalid_trust(void);
int test_ai_mcp_server_registry_add(void);
int test_ai_mcp_server_registry_at(void);
int test_ai_mcp_server_registry_duplicate(void);
int test_ai_mcp_server_registry_find(void);
int test_ai_mcp_server_registry_remove(void);
int test_ai_mcp_server_registry_remove_missing(void);
int test_ai_mcp_server_registry_revision(void);
int test_ai_mcp_session_capabilities(void);
int test_ai_mcp_session_disconnect(void);
int test_ai_mcp_session_init(void);
int test_ai_mcp_session_initialize(void);
int test_ai_mcp_session_initialize_twice(void);
int test_ai_mcp_session_ping(void);
int test_ai_mcp_session_ping_before_ready(void);
int test_ai_mcp_session_transport_failure(void);

int main(void)
{
    if (test_ai_mcp_array_empty() != 0) return 1;
    if (test_ai_mcp_array_malformed() != 0) return 2;
    if (test_ai_mcp_array_missing() != 0) return 3;
    if (test_ai_mcp_array_nested_object() != 0) return 4;
    if (test_ai_mcp_array_two_objects() != 0) return 5;
    if (test_ai_mcp_bridge_invoke() != 0) return 6;
    if (test_ai_mcp_bridge_policy_denied() != 0) return 7;
    if (test_ai_mcp_bridge_register() != 0) return 8;
    if (test_ai_mcp_call_before_ready() != 0) return 9;
    if (test_ai_mcp_call_tool() != 0) return 10;
    if (test_ai_mcp_catalogue_prompt_add() != 0) return 11;
    if (test_ai_mcp_catalogue_prompt_at() != 0) return 12;
    if (test_ai_mcp_catalogue_prompt_duplicate() != 0) return 13;
    if (test_ai_mcp_catalogue_prompt_find() != 0) return 14;
    if (test_ai_mcp_catalogue_resource_add() != 0) return 15;
    if (test_ai_mcp_catalogue_resource_at() != 0) return 16;
    if (test_ai_mcp_catalogue_resource_duplicate() != 0) return 17;
    if (test_ai_mcp_catalogue_resource_find() != 0) return 18;
    if (test_ai_mcp_catalogue_tool_add() != 0) return 19;
    if (test_ai_mcp_catalogue_tool_at() != 0) return 20;
    if (test_ai_mcp_catalogue_tool_duplicate() != 0) return 21;
    if (test_ai_mcp_catalogue_tool_find() != 0) return 22;
    if (test_ai_mcp_client_discover() != 0) return 23;
    if (test_ai_mcp_client_init() != 0) return 24;
    if (test_ai_mcp_client_register_tools() != 0) return 25;
    if (test_ai_mcp_client_start() != 0) return 26;
    if (test_ai_mcp_codec_encode_capacity() != 0) return 27;
    if (test_ai_mcp_codec_encode_initialize() != 0) return 28;
    if (test_ai_mcp_codec_encode_prompt() != 0) return 29;
    if (test_ai_mcp_codec_encode_resource() != 0) return 30;
    if (test_ai_mcp_codec_encode_tool() != 0) return 31;
    if (test_ai_mcp_codec_initialize() != 0) return 32;
    if (test_ai_mcp_codec_prompt() != 0) return 33;
    if (test_ai_mcp_codec_resource() != 0) return 34;
    if (test_ai_mcp_codec_tool() != 0) return 35;
    if (test_ai_mcp_codec_tool_missing_name() != 0) return 36;
    if (test_ai_mcp_discover_before_ready() != 0) return 37;
    if (test_ai_mcp_discover_prompts() != 0) return 38;
    if (test_ai_mcp_discover_resources() != 0) return 39;
    if (test_ai_mcp_discover_tools() != 0) return 40;
    if (test_ai_mcp_get_prompt() != 0) return 41;
    if (test_ai_mcp_json_escape() != 0) return 42;
    if (test_ai_mcp_json_escape_capacity() != 0) return 43;
    if (test_ai_mcp_json_escaped_string() != 0) return 44;
    if (test_ai_mcp_json_has_key() != 0) return 45;
    if (test_ai_mcp_json_invalid_string() != 0) return 46;
    if (test_ai_mcp_json_missing_string() != 0) return 47;
    if (test_ai_mcp_json_raw_array() != 0) return 48;
    if (test_ai_mcp_json_raw_capacity() != 0) return 49;
    if (test_ai_mcp_json_raw_object() != 0) return 50;
    if (test_ai_mcp_json_string() != 0) return 51;
    if (test_ai_mcp_protocol_initialize() != 0) return 52;
    if (test_ai_mcp_protocol_initialized() != 0) return 53;
    if (test_ai_mcp_protocol_ping() != 0) return 54;
    if (test_ai_mcp_protocol_prompts_get() != 0) return 55;
    if (test_ai_mcp_protocol_prompts_list() != 0) return 56;
    if (test_ai_mcp_protocol_resources_list() != 0) return 57;
    if (test_ai_mcp_protocol_resources_read() != 0) return 58;
    if (test_ai_mcp_protocol_tools_call() != 0) return 59;
    if (test_ai_mcp_protocol_tools_list() != 0) return 60;
    if (test_ai_mcp_read_resource() != 0) return 61;
    if (test_ai_mcp_server_init() != 0) return 62;
    if (test_ai_mcp_server_invalid_id() != 0) return 63;
    if (test_ai_mcp_server_invalid_trust() != 0) return 64;
    if (test_ai_mcp_server_registry_add() != 0) return 65;
    if (test_ai_mcp_server_registry_at() != 0) return 66;
    if (test_ai_mcp_server_registry_duplicate() != 0) return 67;
    if (test_ai_mcp_server_registry_find() != 0) return 68;
    if (test_ai_mcp_server_registry_remove() != 0) return 69;
    if (test_ai_mcp_server_registry_remove_missing() != 0) return 70;
    if (test_ai_mcp_server_registry_revision() != 0) return 71;
    if (test_ai_mcp_session_capabilities() != 0) return 72;
    if (test_ai_mcp_session_disconnect() != 0) return 73;
    if (test_ai_mcp_session_init() != 0) return 74;
    if (test_ai_mcp_session_initialize() != 0) return 75;
    if (test_ai_mcp_session_initialize_twice() != 0) return 76;
    if (test_ai_mcp_session_ping() != 0) return 77;
    if (test_ai_mcp_session_ping_before_ready() != 0) return 78;
    if (test_ai_mcp_session_transport_failure() != 0) return 79;
    puts("AI MCP tests passed");
    return 0;
}
