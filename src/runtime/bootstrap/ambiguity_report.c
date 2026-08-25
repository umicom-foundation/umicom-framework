/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/ambiguity_report.c
 *
 * PURPOSE:
 *   Detect equally-ranked bindings that make a service resolution ambiguous.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/ambiguity_report.h"
#include "umicom/runtime/bootstrap/binding_registry.h"
#include "umicom/runtime/bootstrap/service_key.h"


size_t umi_bootstrap_binding_ambiguity_count(
    const UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapServiceKey *key) {
    const UmiBootstrapBindingDescriptor *best;
    size_t count = 0U;
    size_t i;
    if (registry == NULL || key == NULL) return 0U;
    best = umi_bootstrap_binding_registry_find_best(registry, key);
    if (best == NULL) return 0U;
    for (i = 0U; i < registry->count; ++i) {
        const UmiBootstrapBindingDescriptor *item = &registry->items[i];
        if (!item->enabled || !umi_bootstrap_service_key_equal(&item->key, key)) continue;
        if (item->primary == best->primary && item->priority == best->priority) ++count;
    }
    return count > 1U ? count : 0U;
}
