/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/audit.h
 *
 * PURPOSE:
 *   Record release-pipeline audit events with an actor and stage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Release changes become explainable because each important transition is recorded with who or what performed it.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_AUDIT_H
#define INCLUDE_UMICOM_DELIVERY_AUDIT_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeliveryAuditEvent {
    char event_id[UMI_DELIVERY_ID_CAPACITY];
    char actor_id[UMI_DELIVERY_ID_CAPACITY];
    UmiDeliveryStage stage;
    char message[UMI_DELIVERY_TEXT_CAPACITY];
} UmiDeliveryAuditEvent;
UmiStatus umi_delivery_audit_event_init(UmiDeliveryAuditEvent *event,
                                        const char *event_id,
                                        const char *actor_id,
                                        UmiDeliveryStage stage,
                                        const char *message);

#ifdef __cplusplus
}
#endif

#endif
