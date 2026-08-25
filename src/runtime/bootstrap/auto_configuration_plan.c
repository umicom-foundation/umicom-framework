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
void umi_bootstrap_auto_configuration_plan_init(
    UmiBootstrapAutoConfigurationPlan *plan) {
    if (plan != NULL) memset(plan, 0, sizeof(*plan));
}
static int config_compare(const UmiBootstrapAutoConfiguration *a,
                          const UmiBootstrapAutoConfiguration *b) {
    if (a->priority != b->priority) return a->priority > b->priority ? -1 : 1;
    return umi_bootstrap_text_compare(a->configuration_id, b->configuration_id);
}
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
    if (catalogue == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_plan, 0, sizeof(*out_plan));
    for (i = 0U; i < catalogue->count; ++i) {
        const UmiBootstrapAutoConfiguration *item = &catalogue->items[i];
        if (!item->enabled) continue;
        if (item->conditions.count == 0U ||
            umi_bootstrap_condition_set_evaluate(&item->conditions, properties,
                environment, features, capabilities, platform_id)) {
            selected[selected_count++] = i;
        }
    }
    for (i = 1U; i < selected_count; ++i) {
        size_t value = selected[i];
        j = i;
        while (j > 0U &&
               config_compare(&catalogue->items[value],
                              &catalogue->items[selected[j - 1U]]) < 0) {
            selected[j] = selected[j - 1U];
            --j;
        }
        selected[j] = value;
    }
    for (i = 0U; i < selected_count; ++i) {
        (void)umi_bootstrap_copy_text(out_plan->configuration_ids[out_plan->count++],
            UMI_BOOTSTRAP_ID_CAPACITY, catalogue->items[selected[i]].configuration_id);
    }
    return UMI_STATUS_OK;
}
