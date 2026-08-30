/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_cases.h
 *
 * PURPOSE:
 *   Declare focused AI/Helix bridge test cases for the suite entry point.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef TESTS_AI_HELIX_BRIDGE_TEST_CASES_H
#define TESTS_AI_HELIX_BRIDGE_TEST_CASES_H

int test_bridge_cancelled_finish(void);
int test_bridge_complete(void);
int test_bridge_context_limit(void);
int test_bridge_length_finish(void);
int test_bridge_parse_error(void);
int test_bridge_proposal(void);
int test_bridge_provider_policy_denied(void);
int test_bridge_role_denied(void);
int test_bridge_source_control_opt_in(void);
int test_bridge_tools_disabled(void);
int test_bridge_usage_preserved(void);
int test_config_defaults(void);
int test_config_empty_provider(void);
int test_config_temperature_range(void);
int test_config_zero_tokens(void);
int test_evidence_add(void);
int test_evidence_capacity_atomic(void);
int test_evidence_init(void);
int test_evidence_multiple(void);
int test_parser_confidence_range(void);
int test_parser_crlf(void);
int test_parser_duplicate_field(void);
int test_parser_duplicate_relaxed(void);
int test_parser_invalid_action(void);
int test_parser_invalid_confidence(void);
int test_parser_missing_field(void);
int test_parser_valid_complete(void);
int test_parser_valid_proposal(void);
int test_prompt_system_protocol(void);
int test_prompt_system_role(void);
int test_prompt_user_evidence_boundary(void);
int test_prompt_user_objective(void);
int test_protocol_action_build(void);
int test_protocol_action_filesystem(void);
int test_protocol_action_invalid(void);
int test_protocol_action_none(void);
int test_protocol_action_read(void);
int test_protocol_action_review(void);
int test_protocol_action_source_control(void);
int test_protocol_action_test(void);
int test_protocol_disposition_complete(void);
int test_protocol_disposition_context(void);
int test_protocol_disposition_defer(void);
int test_protocol_disposition_invalid(void);
int test_protocol_disposition_propose(void);
int test_role_builder_build(void);
int test_role_builder_filesystem(void);
int test_role_builder_source_control(void);
int test_role_designer_filesystem(void);
int test_role_designer_read(void);
int test_role_reviewer_test(void);
int test_role_suggestion_review(void);
int test_role_test_build(void);
int test_role_unknown(void);
int test_validator_builder_filesystem(void);
int test_validator_config_action_denied(void);
int test_validator_designer_filesystem_denied(void);
int test_validator_nonproposal_action(void);
int test_validator_source_control_default_denied(void);
int test_validator_source_control_explicit(void);
int test_validator_target_required(void);

#endif
