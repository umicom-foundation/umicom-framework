/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/audit.c
 *
 * PURPOSE:
 *   Record release-pipeline audit events with an actor and stage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Release changes become explainable because each important transition is recorded with who or what performed it.
 */

#include "umicom/delivery/audit.h"
#include "../base/value_construction_internal.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise delivery audit event from caller-provided values so later operations receive
 * a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_delivery_audit_event_init(UmiDeliveryAuditEvent *event,
                                        const char *event_id,
                                        const char *actor_id,
                                        UmiDeliveryStage stage,
                                        const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL || event_id == NULL || actor_id == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(event, 0, sizeof(*event));
    event->stage = stage;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(event->event_id, sizeof(event->event_id), event_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(event->actor_id, sizeof(event->actor_id), actor_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(event->message, sizeof(event->message), message) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_delivery_audit_event_init_build_value(UmiDeliveryAuditEvent *event,
                                        const char *event_id,
                                        const char *actor_id,
                                        UmiDeliveryStage stage,
                                        const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL || event_id == NULL || actor_id == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(event, 0, sizeof(*event));
    event->stage = stage;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(event->event_id, sizeof(event->event_id), event_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(event->actor_id, sizeof(event->actor_id), actor_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(event->message, sizeof(event->message), message) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_delivery_audit_event_init, UmiDeliveryAuditEvent, event,
    (UmiDeliveryAuditEvent *event, const char *event_id, const char *actor_id, UmiDeliveryStage stage, const char *message),
    umi_delivery_audit_event_init_build_value(candidate, event_id, actor_id, stage, message))
