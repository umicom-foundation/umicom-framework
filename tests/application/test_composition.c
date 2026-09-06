/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_composition.c
 *
 * PURPOSE:
 *   Verify a thin product can combine its own definition with reusable
 *   Framework presets, packs and component domains without copying services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/composition.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    static const char *const required[] = {
        "umicom.runtime",
        "umicom.application"
    };
    static const char *const optional[] = {
        "umicom.codeguard"
    };
    static const char *const domains[] = {
        "development"
    };
    static const char *const profiles[] = {
        "developer.default"
    };
    static const char *const extra_packs[] = {
        "umicom.pack.ai-assistant"
    };

    UmiApplicationDefinition definition;
    UmiApplicationCompositionRequest request;
    UmiApplicationCompositionPlan *plan;

    /* The plan owns several bounded inventories, so allocate it on the heap
     * instead of consuming the small native stack used by Windows test hosts. */
    plan = (UmiApplicationCompositionPlan *)calloc(1U, sizeof(*plan));
    if (plan == NULL) {
        (void)fprintf(stderr, "could not allocate composition plan\n");
        return EXIT_FAILURE;
    }

    (void)memset(&definition, 0, sizeof(definition));
    definition.structure_size = (uint32_t)sizeof(definition);
    definition.application_id = "org.umicom.sample";
    definition.display_name = "Umicom Sample";
    definition.repository_slug = "umicom-sample";
    definition.executable_name = "umicom-sample";
    definition.purpose =
        "Verify Framework-first thin application composition.";
    definition.family = UMI_APPLICATION_FAMILY_DEVELOPMENT;
    definition.maturity = UMI_APPLICATION_FOUNDATION;
    definition.frontends = UMI_FRONTEND_CONSOLE | UMI_FRONTEND_GTK4;
    definition.flags =
        UMI_APPLICATION_STANDALONE |
        UMI_APPLICATION_FEDERATED;
    definition.required_capabilities = required;
    definition.required_capability_count =
        sizeof(required) / sizeof(required[0]);
    definition.optional_capabilities = optional;
    definition.optional_capability_count =
        sizeof(optional) / sizeof(optional[0]);
    definition.component_domains = domains;
    definition.component_domain_count =
        sizeof(domains) / sizeof(domains[0]);
    definition.workspace_profiles = profiles;
    definition.workspace_profile_count =
        sizeof(profiles) / sizeof(profiles[0]);

    umi_application_composition_request_init(&request, &definition);
    request.preset_id = "umicom.preset.developer-workbench";
    request.additional_pack_ids = extra_packs;
    request.additional_pack_count =
        sizeof(extra_packs) / sizeof(extra_packs[0]);
    request.include_definition_domains = 1;

    if (umi_application_composition_build(&request, plan) != UMI_STATUS_OK) {
        (void)fprintf(stderr, "composition build failed\n");
        free(plan);
        return EXIT_FAILURE;
    }

    if (strcmp(plan->application_id, "org.umicom.sample") != 0 ||
        !umi_application_composition_has_pack(
            plan, "umicom.pack.core-runtime") ||
        !umi_application_composition_has_pack(
            plan, "umicom.pack.developer") ||
        !umi_application_composition_has_pack(
            plan, "umicom.pack.ai-assistant") ||
        !umi_application_composition_has_capability(plan, "umicom.build") ||
        !umi_application_composition_has_capability(
            plan, "umicom.ai.coding-assistant") ||
        !umi_application_composition_has_component(
            plan, "umicom.development.explorer") ||
        !umi_application_composition_has_component(plan, "umicom.development.build") ||
        !umi_application_composition_has_component(plan, "umicom.ai.chat") ||
        plan->pack_count == 0U || plan->required_capability_count == 0U ||
        plan->component_count == 0U || plan->implemented_capability_count == 0U) {
        (void)fprintf(stderr, "composition plan does not satisfy its contract\n");
        free(plan);
        return EXIT_FAILURE;
    }
    free(plan);
    return 0;
}
