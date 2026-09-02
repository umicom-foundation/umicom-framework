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

/**
 * Exercise test bridge cancelled finish and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_bridge_cancelled_finish(void);
/**
 * Exercise test bridge complete and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_bridge_complete(void);
/**
 * Exercise test bridge context limit and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_bridge_context_limit(void);
/**
 * Exercise test bridge length finish and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_bridge_length_finish(void);
/**
 * Exercise test bridge parse error and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_bridge_parse_error(void);
/**
 * Exercise test bridge proposal and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_bridge_proposal(void);
/**
 * Exercise test bridge provider policy denied and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_bridge_provider_policy_denied(void);
/**
 * Exercise test bridge role denied and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_bridge_role_denied(void);
/**
 * Exercise test bridge source control opt in and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_bridge_source_control_opt_in(void);
/**
 * Exercise test bridge tools disabled and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_bridge_tools_disabled(void);
/**
 * Exercise test bridge usage preserved and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_bridge_usage_preserved(void);
/**
 * Exercise test config defaults and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_config_defaults(void);
/**
 * Exercise test config empty provider and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_config_empty_provider(void);
/**
 * Exercise test config temperature range and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_config_temperature_range(void);
/**
 * Exercise test config zero tokens and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_config_zero_tokens(void);
/**
 * Add test evidence only after its inputs and available capacity have been checked.
 */
int test_evidence_add(void);
/**
 * Exercise test evidence capacity atomic and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_evidence_capacity_atomic(void);
/**
 * Initialise test evidence from caller-provided values so later operations receive a known
 * state.
 */
int test_evidence_init(void);
/**
 * Exercise test evidence multiple and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_evidence_multiple(void);
/**
 * Exercise test parser confidence range and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_parser_confidence_range(void);
/**
 * Exercise test parser crlf and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_parser_crlf(void);
/**
 * Exercise test parser duplicate field and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_parser_duplicate_field(void);
/**
 * Exercise test parser duplicate relaxed and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_parser_duplicate_relaxed(void);
/**
 * Exercise test parser invalid action and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_parser_invalid_action(void);
/**
 * Exercise test parser invalid confidence and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_parser_invalid_confidence(void);
/**
 * Exercise test parser missing field and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_parser_missing_field(void);
/**
 * Exercise test parser valid complete and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_parser_valid_complete(void);
/**
 * Exercise test parser valid proposal and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_parser_valid_proposal(void);
/**
 * Exercise test prompt system protocol and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_prompt_system_protocol(void);
/**
 * Exercise test prompt system role and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_prompt_system_role(void);
/**
 * Exercise test prompt user evidence boundary and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_prompt_user_evidence_boundary(void);
/**
 * Exercise test prompt user objective and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_prompt_user_objective(void);
/**
 * Exercise test protocol action build and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_protocol_action_build(void);
/**
 * Exercise test protocol action filesystem and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_protocol_action_filesystem(void);
/**
 * Exercise test protocol action invalid and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_protocol_action_invalid(void);
/**
 * Exercise test protocol action none and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_protocol_action_none(void);
/**
 * Read test protocol action into validated module state and return a status when input
 * cannot be used.
 */
int test_protocol_action_read(void);
/**
 * Exercise test protocol action review and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_protocol_action_review(void);
/**
 * Exercise test protocol action source control and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_protocol_action_source_control(void);
/**
 * Exercise test protocol action test and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_protocol_action_test(void);
/**
 * Exercise test protocol disposition complete and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_protocol_disposition_complete(void);
/**
 * Exercise test protocol disposition context and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_protocol_disposition_context(void);
/**
 * Exercise test protocol disposition defer and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_protocol_disposition_defer(void);
/**
 * Exercise test protocol disposition invalid and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_protocol_disposition_invalid(void);
/**
 * Exercise test protocol disposition propose and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_protocol_disposition_propose(void);
/**
 * Exercise test role builder build and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_role_builder_build(void);
/**
 * Exercise test role builder filesystem and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_role_builder_filesystem(void);
/**
 * Exercise test role builder source control and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_role_builder_source_control(void);
/**
 * Exercise test role designer filesystem and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_role_designer_filesystem(void);
/**
 * Read test role designer into validated module state and return a status when input
 * cannot be used.
 */
int test_role_designer_read(void);
/**
 * Exercise test role reviewer test and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_role_reviewer_test(void);
/**
 * Exercise test role suggestion review and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_role_suggestion_review(void);
/**
 * Exercise test role test build and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_role_test_build(void);
/**
 * Exercise test role unknown and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_role_unknown(void);
/**
 * Exercise test validator builder filesystem and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_validator_builder_filesystem(void);
/**
 * Exercise test validator config action denied and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_validator_config_action_denied(void);
/**
 * Exercise test validator designer filesystem denied and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_validator_designer_filesystem_denied(void);
/**
 * Exercise test validator nonproposal action and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_validator_nonproposal_action(void);
/**
 * Exercise test validator source control default denied and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_validator_source_control_default_denied(void);
/**
 * Exercise test validator source control explicit and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_validator_source_control_explicit(void);
/**
 * Exercise test validator target required and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_validator_target_required(void);

#endif
