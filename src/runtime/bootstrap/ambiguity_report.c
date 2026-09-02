/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/ambiguity_report.c
 *
 * PURPOSE:
 *   Implement the ambiguity report behavior for
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
 * File: src/runtime/bootstrap/ambiguity_report.c
 *
 * PURPOSE:
 *   Detect equally-ranked bindings that make a service resolution ambiguous.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/ambiguity_report.h"
#include "umicom/runtime/bootstrap/binding_registry.h"
#include "umicom/runtime/bootstrap/service_key.h"


/*
 * Return the number of records represented by bootstrap binding ambiguity without changing
 * their state.
 */
size_t umi_bootstrap_binding_ambiguity_count(
    const UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapServiceKey *key) {
    const UmiBootstrapBindingDescriptor *best;
    size_t count = 0U;
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || key == NULL) return 0U;
    best = umi_bootstrap_binding_registry_find_best(registry, key);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (best == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        const UmiBootstrapBindingDescriptor *item = &registry->items[i];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!item->enabled || !umi_bootstrap_service_key_equal(&item->key, key)) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (item->primary == best->primary && item->priority == best->priority) ++count;
    }
    return count > 1U ? count : 0U;
}
