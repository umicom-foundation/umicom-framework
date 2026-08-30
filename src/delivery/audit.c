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

/* BEGINNER NOTE:
 * Release changes become explainable because each important transition is recorded with who or what performed it.
 */

#include "umicom/delivery/audit.h"
#include "delivery_internal.h"
#include <string.h>
UmiStatus umi_delivery_audit_event_init(UmiDeliveryAuditEvent *event,
                                        const char *event_id,
                                        const char *actor_id,
                                        UmiDeliveryStage stage,
                                        const char *message)
{
    if (event == NULL || event_id == NULL || actor_id == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(event, 0, sizeof(*event));
    event->stage = stage;
    if (umi_delivery_copy_text(event->event_id, sizeof(event->event_id), event_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(event->actor_id, sizeof(event->actor_id), actor_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(event->message, sizeof(event->message), message) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
