/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_main.c
 *
 * PURPOSE:
 *   Run focused MCP protocol, transport, discovery, invocation, bridge and policy-integration tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

/*
 * Exercise test ai mcp array empty and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_array_empty(void);
/*
 * Exercise test ai mcp array malformed and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_array_malformed(void);
/*
 * Exercise test ai mcp array missing and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_array_missing(void);
/*
 * Exercise test ai mcp array nested object and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_array_nested_object(void);
/*
 * Exercise test ai mcp array two objects and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_array_two_objects(void);
/*
 * Exercise test ai mcp bridge invoke and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_bridge_invoke(void);
/*
 * Exercise test ai mcp bridge policy denied and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_bridge_policy_denied(void);
/* Add test ai mcp bridge only after its inputs and available capacity have been checked. */
int test_ai_mcp_bridge_register(void);
/*
 * Exercise test ai mcp call before ready and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_call_before_ready(void);
/*
 * Exercise test ai mcp call tool and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_call_tool(void);
/*
 * Add test ai mcp catalogue prompt only after its inputs and available capacity have been
 * checked.
 */
int test_ai_mcp_catalogue_prompt_add(void);
/*
 * Find test ai mcp catalogue prompt while leaving the underlying catalogue or model owned
 * by this module.
 */
int test_ai_mcp_catalogue_prompt_at(void);
/*
 * Exercise test ai mcp catalogue prompt duplicate and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_catalogue_prompt_duplicate(void);
/*
 * Find test ai mcp catalogue prompt while leaving the underlying catalogue or model owned
 * by this module.
 */
int test_ai_mcp_catalogue_prompt_find(void);
/*
 * Add test ai mcp catalogue resource only after its inputs and available capacity have
 * been checked.
 */
int test_ai_mcp_catalogue_resource_add(void);
/*
 * Find test ai mcp catalogue resource while leaving the underlying catalogue or model
 * owned by this module.
 */
int test_ai_mcp_catalogue_resource_at(void);
/*
 * Exercise test ai mcp catalogue resource duplicate and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_catalogue_resource_duplicate(void);
/*
 * Find test ai mcp catalogue resource while leaving the underlying catalogue or model
 * owned by this module.
 */
int test_ai_mcp_catalogue_resource_find(void);
/*
 * Add test ai mcp catalogue tool only after its inputs and available capacity have been
 * checked.
 */
int test_ai_mcp_catalogue_tool_add(void);
/*
 * Find test ai mcp catalogue tool while leaving the underlying catalogue or model owned by
 * this module.
 */
int test_ai_mcp_catalogue_tool_at(void);
/*
 * Exercise test ai mcp catalogue tool duplicate and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_catalogue_tool_duplicate(void);
/*
 * Find test ai mcp catalogue tool while leaving the underlying catalogue or model owned by
 * this module.
 */
int test_ai_mcp_catalogue_tool_find(void);
/*
 * Exercise test ai mcp client discover and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_client_discover(void);
/*
 * Initialise test ai mcp client from caller-provided values so later operations receive a
 * known state.
 */
int test_ai_mcp_client_init(void);
/*
 * Exercise test ai mcp client register tools and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_client_register_tools(void);
/*
 * Exercise test ai mcp client start and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_client_start(void);
/*
 * Exercise test ai mcp codec encode capacity and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_codec_encode_capacity(void);
/*
 * Initialise test ai mcp codec encode from caller-provided values so later operations
 * receive a known state.
 */
int test_ai_mcp_codec_encode_initialize(void);
/*
 * Exercise test ai mcp codec encode prompt and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_codec_encode_prompt(void);
/*
 * Exercise test ai mcp codec encode resource and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_codec_encode_resource(void);
/*
 * Exercise test ai mcp codec encode tool and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_codec_encode_tool(void);
/*
 * Initialise test ai mcp codec from caller-provided values so later operations receive a
 * known state.
 */
int test_ai_mcp_codec_initialize(void);
/*
 * Exercise test ai mcp codec prompt and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_codec_prompt(void);
/*
 * Exercise test ai mcp codec resource and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_codec_resource(void);
/*
 * Exercise test ai mcp codec tool and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_codec_tool(void);
/*
 * Exercise test ai mcp codec tool missing name and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_codec_tool_missing_name(void);
/*
 * Exercise test ai mcp discover before ready and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_discover_before_ready(void);
/*
 * Exercise test ai mcp discover prompts and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_discover_prompts(void);
/*
 * Exercise test ai mcp discover resources and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_discover_resources(void);
/*
 * Exercise test ai mcp discover tools and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_discover_tools(void);
/*
 * Exercise test ai mcp get prompt and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_get_prompt(void);
/*
 * Exercise test ai mcp json escape and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_json_escape(void);
/*
 * Exercise test ai mcp json escape capacity and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_json_escape_capacity(void);
/*
 * Exercise test ai mcp json escaped string and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_json_escaped_string(void);
/*
 * Exercise test ai mcp json has key and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_json_has_key(void);
/*
 * Exercise test ai mcp json invalid string and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_json_invalid_string(void);
/*
 * Exercise test ai mcp json missing string and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_json_missing_string(void);
/*
 * Exercise test ai mcp json raw array and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_json_raw_array(void);
/*
 * Exercise test ai mcp json raw capacity and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_json_raw_capacity(void);
/*
 * Exercise test ai mcp json raw object and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_json_raw_object(void);
/*
 * Exercise test ai mcp json string and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_json_string(void);
/*
 * Initialise test ai mcp protocol from caller-provided values so later operations receive
 * a known state.
 */
int test_ai_mcp_protocol_initialize(void);
/*
 * Exercise test ai mcp protocol initialized and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_protocol_initialized(void);
/*
 * Exercise test ai mcp protocol ping and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_protocol_ping(void);
/*
 * Exercise test ai mcp protocol prompts get and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_protocol_prompts_get(void);
/*
 * Exercise test ai mcp protocol prompts list and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_protocol_prompts_list(void);
/*
 * Exercise test ai mcp protocol resources list and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_protocol_resources_list(void);
/*
 * Read test ai mcp protocol resources into validated module state and return a status when
 * input cannot be used.
 */
int test_ai_mcp_protocol_resources_read(void);
/*
 * Exercise test ai mcp protocol tools call and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_protocol_tools_call(void);
/*
 * Exercise test ai mcp protocol tools list and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_protocol_tools_list(void);
/*
 * Exercise test ai mcp read resource and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_read_resource(void);
/*
 * Initialise test ai mcp server from caller-provided values so later operations receive a
 * known state.
 */
int test_ai_mcp_server_init(void);
/*
 * Exercise test ai mcp server invalid id and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_server_invalid_id(void);
/*
 * Exercise test ai mcp server invalid trust and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_server_invalid_trust(void);
/*
 * Add test ai mcp server registry only after its inputs and available capacity have been
 * checked.
 */
int test_ai_mcp_server_registry_add(void);
/*
 * Find test ai mcp server registry while leaving the underlying catalogue or model owned
 * by this module.
 */
int test_ai_mcp_server_registry_at(void);
/*
 * Exercise test ai mcp server registry duplicate and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_server_registry_duplicate(void);
/*
 * Find test ai mcp server registry while leaving the underlying catalogue or model owned
 * by this module.
 */
int test_ai_mcp_server_registry_find(void);
/*
 * Remove test ai mcp server registry while keeping the remaining records in a valid and
 * discoverable state.
 */
int test_ai_mcp_server_registry_remove(void);
/*
 * Exercise test ai mcp server registry remove missing and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_server_registry_remove_missing(void);
/*
 * Exercise test ai mcp server registry revision and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_server_registry_revision(void);
/*
 * Exercise test ai mcp session capabilities and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_session_capabilities(void);
/*
 * Exercise test ai mcp session disconnect and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_session_disconnect(void);
/*
 * Initialise test ai mcp session from caller-provided values so later operations receive a
 * known state.
 */
int test_ai_mcp_session_init(void);
/*
 * Initialise test ai mcp session from caller-provided values so later operations receive a
 * known state.
 */
int test_ai_mcp_session_initialize(void);
/*
 * Exercise test ai mcp session initialize twice and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_session_initialize_twice(void);
/*
 * Exercise test ai mcp session ping and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_session_ping(void);
/*
 * Exercise test ai mcp session ping before ready and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_session_ping_before_ready(void);
/*
 * Exercise test ai mcp session transport failure and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_session_transport_failure(void);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_array_empty() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_array_malformed() != 0) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_array_missing() != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_array_nested_object() != 0) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_array_two_objects() != 0) return 5;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (test_ai_mcp_bridge_invoke() != 0) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_bridge_policy_denied() != 0) return 7;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_bridge_register() != 0) return 8;
    /* Apply this operation only while the related capability or state is available. */
    if (test_ai_mcp_call_before_ready() != 0) return 9;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_call_tool() != 0) return 10;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_prompt_add() != 0) return 11;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_prompt_at() != 0) return 12;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_prompt_duplicate() != 0) return 13;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_prompt_find() != 0) return 14;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_resource_add() != 0) return 15;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_resource_at() != 0) return 16;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_resource_duplicate() != 0) return 17;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_resource_find() != 0) return 18;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_tool_add() != 0) return 19;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_tool_at() != 0) return 20;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_tool_duplicate() != 0) return 21;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_catalogue_tool_find() != 0) return 22;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_client_discover() != 0) return 23;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_client_init() != 0) return 24;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_client_register_tools() != 0) return 25;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_client_start() != 0) return 26;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_codec_encode_capacity() != 0) return 27;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_codec_encode_initialize() != 0) return 28;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_codec_encode_prompt() != 0) return 29;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_codec_encode_resource() != 0) return 30;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_codec_encode_tool() != 0) return 31;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_codec_initialize() != 0) return 32;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_codec_prompt() != 0) return 33;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_codec_resource() != 0) return 34;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_codec_tool() != 0) return 35;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_codec_tool_missing_name() != 0) return 36;
    /* Apply this operation only while the related capability or state is available. */
    if (test_ai_mcp_discover_before_ready() != 0) return 37;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_discover_prompts() != 0) return 38;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_discover_resources() != 0) return 39;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_discover_tools() != 0) return 40;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_get_prompt() != 0) return 41;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_json_escape() != 0) return 42;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_json_escape_capacity() != 0) return 43;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_json_escaped_string() != 0) return 44;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_json_has_key() != 0) return 45;
    /* Apply this operation only while the related capability or state is available. */
    if (test_ai_mcp_json_invalid_string() != 0) return 46;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_json_missing_string() != 0) return 47;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_json_raw_array() != 0) return 48;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_json_raw_capacity() != 0) return 49;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_json_raw_object() != 0) return 50;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_json_string() != 0) return 51;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_protocol_initialize() != 0) return 52;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_protocol_initialized() != 0) return 53;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_protocol_ping() != 0) return 54;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_protocol_prompts_get() != 0) return 55;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_protocol_prompts_list() != 0) return 56;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_protocol_resources_list() != 0) return 57;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_protocol_resources_read() != 0) return 58;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_protocol_tools_call() != 0) return 59;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_protocol_tools_list() != 0) return 60;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_read_resource() != 0) return 61;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_server_init() != 0) return 62;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (test_ai_mcp_server_invalid_id() != 0) return 63;
    /* Apply this operation only while the related capability or state is available. */
    if (test_ai_mcp_server_invalid_trust() != 0) return 64;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_server_registry_add() != 0) return 65;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_server_registry_at() != 0) return 66;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_server_registry_duplicate() != 0) return 67;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_server_registry_find() != 0) return 68;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_server_registry_remove() != 0) return 69;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_server_registry_remove_missing() != 0) return 70;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_server_registry_revision() != 0) return 71;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_session_capabilities() != 0) return 72;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_session_disconnect() != 0) return 73;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_session_init() != 0) return 74;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_session_initialize() != 0) return 75;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_session_initialize_twice() != 0) return 76;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_session_ping() != 0) return 77;
    /* Apply this operation only while the related capability or state is available. */
    if (test_ai_mcp_session_ping_before_ready() != 0) return 78;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_mcp_session_transport_failure() != 0) return 79;
    puts("AI MCP tests passed");
    return 0;
}
