/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/message_contract.h
 *
 * PURPOSE:
 *   Describe a versioned message schema, payload ceiling and delivery semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_MESSAGE_CONTRACT_H
#define UMICOM_INTEGRATION_FABRIC_MESSAGE_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric message contract data shared with callers of this public contract.
 */
typedef struct UmiFabricMessageContract {
    char contract_id[UMI_FABRIC_ID_CAPACITY];
    char schema_id[UMI_FABRIC_ID_CAPACITY];
    UmiFabricVersion version;
    uint64_t max_payload_bytes;
    UmiFabricDeliveryMode delivery_mode;
} UmiFabricMessageContract;

/**
 * Initialise fabric message contract from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_message_contract_init(UmiFabricMessageContract *item, const char *contract_id, const char *schema_id, UmiFabricVersion version, uint64_t max_payload_bytes, UmiFabricDeliveryMode delivery_mode);
/**
 * Check that fabric message contract satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_message_contract_validate(const UmiFabricMessageContract *item);

#ifdef __cplusplus
}
#endif
#endif
