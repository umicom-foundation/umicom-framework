/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/delivery_policy.h
 *
 * PURPOSE:
 *   Describe acknowledgement, attempt and durability requirements for message delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_DELIVERY_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_DELIVERY_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricDeliveryPolicy {
    char policy_id[UMI_FABRIC_ID_CAPACITY];
    UmiFabricDeliveryMode mode;
    uint32_t max_attempts;
    uint64_t acknowledgement_timeout_ms;
    bool durable;
} UmiFabricDeliveryPolicy;

UmiStatus umi_fabric_delivery_policy_init(UmiFabricDeliveryPolicy *item, const char *policy_id, UmiFabricDeliveryMode mode, uint32_t max_attempts, uint64_t acknowledgement_timeout_ms, bool durable);
UmiStatus umi_fabric_delivery_policy_validate(const UmiFabricDeliveryPolicy *item);

#ifdef __cplusplus
}
#endif
#endif
