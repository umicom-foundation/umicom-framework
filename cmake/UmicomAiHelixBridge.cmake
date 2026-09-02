#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiHelixBridge.cmake
#
# PURPOSE:
#   Extend the existing AI target with a governed reasoning bridge to the
#   existing Helix specialist-agent contracts.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_ai)
    message(FATAL_ERROR
        "UmicomAiHelixBridge requires existing umicom_ai")
endif()

# This bridge performs model inference only. It does not create another agent,
# tool, permission or execution system. The returned UmiHelixAction remains
# subject to Helix approval gates and the existing host execution adapter.
target_sources(umicom_ai PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/helix/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/helix/bridge_config.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/helix/role_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/helix/protocol.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/helix/prompt_builder.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/helix/response_parser.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/helix/proposal_validator.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/helix/evidence_context.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai/helix/bridge.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING AND NOT TARGET umicom-ai-helix-bridge-tests)
    add_executable(umicom-ai-helix-bridge-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_cancelled_finish.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_complete.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_context_limit.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_length_finish.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_parse_error.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_proposal.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_provider_policy_denied.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_role_denied.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_source_control_opt_in.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_tools_disabled.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_bridge_usage_preserved.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_config_defaults.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_config_empty_provider.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_config_temperature_range.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_config_zero_tokens.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_evidence_add.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_evidence_capacity_atomic.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_evidence_init.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_evidence_multiple.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_parser_confidence_range.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_parser_crlf.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_parser_duplicate_field.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_parser_duplicate_relaxed.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_parser_invalid_action.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_parser_invalid_confidence.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_parser_missing_field.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_parser_valid_complete.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_parser_valid_proposal.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_prompt_system_protocol.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_prompt_system_role.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_prompt_user_evidence_boundary.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_prompt_user_objective.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_action_build.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_action_filesystem.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_action_invalid.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_action_none.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_action_read.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_action_review.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_action_source_control.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_action_test.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_disposition_complete.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_disposition_context.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_disposition_defer.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_disposition_invalid.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_protocol_disposition_propose.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_role_builder_build.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_role_builder_filesystem.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_role_builder_source_control.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_role_designer_filesystem.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_role_designer_read.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_role_reviewer_test.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_role_suggestion_review.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_role_test_build.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_role_unknown.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_validator_builder_filesystem.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_validator_config_action_denied.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_validator_designer_filesystem_denied.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_validator_nonproposal_action.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_validator_source_control_default_denied.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_validator_source_control_explicit.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge/test_validator_target_required.c"
    )
    target_include_directories(umicom-ai-helix-bridge-tests PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_helix_bridge")
    target_link_libraries(umicom-ai-helix-bridge-tests PRIVATE
        Umicom::ai)
    umicom_apply_warnings(umicom-ai-helix-bridge-tests)
    umicom_apply_sanitizers(umicom-ai-helix-bridge-tests)
    add_test(
        NAME framework.ai.helix_bridge
        COMMAND umicom-ai-helix-bridge-tests)
endif()
