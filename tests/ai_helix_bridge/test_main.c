/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_main.c
 *
 * PURPOSE:
 *   Execute all focused governed AI/Helix bridge tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_cases.h"
#include <stdio.h>

int main(void)
{
    int failures = 0;
    failures += test_bridge_cancelled_finish();
    failures += test_bridge_complete();
    failures += test_bridge_context_limit();
    failures += test_bridge_length_finish();
    failures += test_bridge_parse_error();
    failures += test_bridge_proposal();
    failures += test_bridge_provider_policy_denied();
    failures += test_bridge_role_denied();
    failures += test_bridge_source_control_opt_in();
    failures += test_bridge_tools_disabled();
    failures += test_bridge_usage_preserved();
    failures += test_config_defaults();
    failures += test_config_empty_provider();
    failures += test_config_temperature_range();
    failures += test_config_zero_tokens();
    failures += test_evidence_add();
    failures += test_evidence_capacity_atomic();
    failures += test_evidence_init();
    failures += test_evidence_multiple();
    failures += test_parser_confidence_range();
    failures += test_parser_crlf();
    failures += test_parser_duplicate_field();
    failures += test_parser_duplicate_relaxed();
    failures += test_parser_invalid_action();
    failures += test_parser_invalid_confidence();
    failures += test_parser_missing_field();
    failures += test_parser_valid_complete();
    failures += test_parser_valid_proposal();
    failures += test_prompt_system_protocol();
    failures += test_prompt_system_role();
    failures += test_prompt_user_evidence_boundary();
    failures += test_prompt_user_objective();
    failures += test_protocol_action_build();
    failures += test_protocol_action_filesystem();
    failures += test_protocol_action_invalid();
    failures += test_protocol_action_none();
    failures += test_protocol_action_read();
    failures += test_protocol_action_review();
    failures += test_protocol_action_source_control();
    failures += test_protocol_action_test();
    failures += test_protocol_disposition_complete();
    failures += test_protocol_disposition_context();
    failures += test_protocol_disposition_defer();
    failures += test_protocol_disposition_invalid();
    failures += test_protocol_disposition_propose();
    failures += test_role_builder_build();
    failures += test_role_builder_filesystem();
    failures += test_role_builder_source_control();
    failures += test_role_designer_filesystem();
    failures += test_role_designer_read();
    failures += test_role_reviewer_test();
    failures += test_role_suggestion_review();
    failures += test_role_test_build();
    failures += test_role_unknown();
    failures += test_validator_builder_filesystem();
    failures += test_validator_config_action_denied();
    failures += test_validator_designer_filesystem_denied();
    failures += test_validator_nonproposal_action();
    failures += test_validator_source_control_default_denied();
    failures += test_validator_source_control_explicit();
    failures += test_validator_target_required();
    if (failures != 0) {
        (void)fprintf(stderr, "AI Helix bridge tests failed: %d\n", failures);
        return 1;
    }
    (void)puts("AI Helix bridge tests passed");
    return 0;
}
