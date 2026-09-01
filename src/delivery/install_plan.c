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

void umi_install_plan_init(UmiInstallPlan *plan)
{
    if (plan != NULL) (void)memset(plan, 0, sizeof(*plan));
}

UmiStatus umi_install_plan_add(UmiInstallPlan *plan,
                               UmiInstallOperationKind kind,
                               const char *source,
                               const char *destination)
{
    UmiInstallOperation *operation;
    if (plan == NULL || destination == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->count >= UMI_DELIVERY_MAX_OPERATIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    operation = &plan->operations[plan->count];
    (void)memset(operation, 0, sizeof(*operation));
    operation->kind = kind;
    if (source != NULL &&
        umi_delivery_copy_text(operation->source, sizeof(operation->source), source) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_delivery_copy_text(operation->destination,
                               sizeof(operation->destination),
                               destination) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++plan->count;
    return UMI_STATUS_OK;
}
