/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/install_plan.c
 *
 * PURPOSE:
 *   Build a bounded sequence of installation operations before modifying an installation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * An install plan is inspectable and testable, which makes package installation safer than scattered file-copy commands.
 */

#include "umicom/delivery/install_plan.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise install plan from caller-provided values so later operations receive a known
 * state.
 */
void umi_install_plan_init(UmiInstallPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan != NULL) (void)memset(plan, 0, sizeof(*plan));
}

/* Add install plan only after its inputs and available capacity have been checked. */
UmiStatus umi_install_plan_add(UmiInstallPlan *plan,
                               UmiInstallOperationKind kind,
                               const char *source,
                               const char *destination)
{
    UmiInstallOperation *operation;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || destination == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->count >= UMI_DELIVERY_MAX_OPERATIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    operation = &plan->operations[plan->count];
    (void)memset(operation, 0, sizeof(*operation));
    operation->kind = kind;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source != NULL &&
        umi_delivery_copy_text(operation->source, sizeof(operation->source), source) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_delivery_copy_text(operation->destination,
                               sizeof(operation->destination),
                               destination) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++plan->count;
    return UMI_STATUS_OK;
}
