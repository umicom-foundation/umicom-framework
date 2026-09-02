/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/auto_configuration_plan.c
 *
 * PURPOSE:
 *   Implement the auto configuration plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/auto_configuration_plan.c
 *
 * PURPOSE:
 *   Select and priority-order applicable auto-configurations.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/auto_configuration_plan.h"
#include "umicom/runtime/bootstrap/condition_evaluator.h"


#include <string.h>
/*
 * Initialise bootstrap auto configuration plan from caller-provided values so later
 * operations receive a known state.
 */
void umi_bootstrap_auto_configuration_plan_init(
    UmiBootstrapAutoConfigurationPlan *plan) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan != NULL) memset(plan, 0, sizeof(*plan));
}
/* Provide the config compare operation used by this module and its client applications. */
static int config_compare(const UmiBootstrapAutoConfiguration *a,
                          const UmiBootstrapAutoConfiguration *b) {
    /* Apply this branch only when its contract condition is satisfied. */
    if (a->priority != b->priority) return a->priority > b->priority ? -1 : 1;
    return umi_bootstrap_text_compare(a->configuration_id, b->configuration_id);
}
/*
 * Provide the bootstrap auto configuration plan build operation used by this module and
 * its client applications.
 */
UmiStatus umi_bootstrap_auto_configuration_plan_build(
    const UmiBootstrapAutoConfigurationCatalogue *catalogue,
    const UmiBootstrapPropertySet *properties,
    const UmiBootstrapPropertySet *environment,
    const UmiBootstrapIdList *features,
    const UmiBootstrapIdList *capabilities,
    const char *platform_id,
    UmiBootstrapAutoConfigurationPlan *out_plan) {
    size_t selected[UMI_BOOTSTRAP_MAX_ITEMS];
    size_t selected_count = 0U;
    size_t i, j;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_plan, 0, sizeof(*out_plan));
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < catalogue->count; ++i) {
        const UmiBootstrapAutoConfiguration *item = &catalogue->items[i];
        /* Apply this operation only while the related capability or state is available. */
        if (!item->enabled) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (item->conditions.count == 0U ||
            umi_bootstrap_condition_set_evaluate(&item->conditions, properties,
                environment, features, capabilities, platform_id)) {
            selected[selected_count++] = i;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 1U; i < selected_count; ++i) {
        size_t value = selected[i];
        j = i;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (j > 0U &&
               config_compare(&catalogue->items[value],
                              &catalogue->items[selected[j - 1U]]) < 0) {
            selected[j] = selected[j - 1U];
            --j;
        }
        selected[j] = value;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < selected_count; ++i) {
        (void)umi_bootstrap_copy_text(out_plan->configuration_ids[out_plan->count++],
            UMI_BOOTSTRAP_ID_CAPACITY, catalogue->items[selected[i]].configuration_id);
    }
    return UMI_STATUS_OK;
}
