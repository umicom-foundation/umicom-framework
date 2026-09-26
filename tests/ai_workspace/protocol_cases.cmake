#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/ai_workspace/protocol_cases.cmake
# PURPOSE: Register named, bounded AI workspace regression cases.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
set(UMICOM_AI_WORKSPACE_PROTOCOL_CASES
    valid_reply
    unicode_reply
    length_finish
    malformed_json
    trailing_json
    empty_choices
    null_content
    wrong_role
    tool_call_rejected
    function_call_rejected
    unsupported_finish
    wrong_model
    embedded_nul
    usage_mismatch
    usage_negative
    error_payload
    plain_action_text_is_inert
    encode_escaping
    encode_tools_disabled
    encode_role_rejected
    encode_temperature_rejected
    encode_count_rejected
    encode_body_capacity
    reply_over_capacity
)
