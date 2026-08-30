/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/types.h
 *
 * PURPOSE:
 *   Define stable C23 capacities, states, protocols and utility helpers shared by the enterprise Integration Fabric.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_TYPES_H
#define UMICOM_INTEGRATION_FABRIC_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_FABRIC_ID_CAPACITY 128U
#define UMI_FABRIC_TEXT_CAPACITY 256U
#define UMI_FABRIC_URI_CAPACITY 512U
#define UMI_FABRIC_MAX_ITEMS 64U
#define UMI_FABRIC_MAX_EDGES 128U
#define UMI_FABRIC_MAX_STEPS 64U
#define UMI_FABRIC_MAX_HEADERS 32U
#define UMI_FABRIC_MAX_DEDUPE 64U

typedef enum UmiFabricState {
    UMI_FABRIC_STATE_UNKNOWN=0,
    UMI_FABRIC_STATE_READY=1,
    UMI_FABRIC_STATE_DEGRADED=2,
    UMI_FABRIC_STATE_BLOCKED=3,
    UMI_FABRIC_STATE_FAILED=4
} UmiFabricState;

typedef enum UmiFabricProtocol {
    UMI_FABRIC_PROTOCOL_INPROC=1,
    UMI_FABRIC_PROTOCOL_HTTP=2,
    UMI_FABRIC_PROTOCOL_WEBSOCKET=3,
    UMI_FABRIC_PROTOCOL_TCP=4,
    UMI_FABRIC_PROTOCOL_MESSAGE_BUS=5,
    UMI_FABRIC_PROTOCOL_FILE=6
} UmiFabricProtocol;

typedef enum UmiFabricDeliveryMode {
    UMI_FABRIC_DELIVERY_AT_MOST_ONCE=1,
    UMI_FABRIC_DELIVERY_AT_LEAST_ONCE=2,
    UMI_FABRIC_DELIVERY_IDEMPOTENT_EFFECT=3
} UmiFabricDeliveryMode;

typedef enum UmiFabricCircuitState {
    UMI_FABRIC_CIRCUIT_CLOSED=1,
    UMI_FABRIC_CIRCUIT_OPEN=2,
    UMI_FABRIC_CIRCUIT_HALF_OPEN=3
} UmiFabricCircuitState;

typedef struct UmiFabricVersion { uint32_t major, minor, patch; } UmiFabricVersion;

/* Copy text into a bounded Framework buffer and always terminate it. */
UmiStatus umi_fabric_copy_text(char *destination,size_t capacity,const char *source);
/* Produce deterministic non-cryptographic identity evidence for plans and snapshots. */
uint64_t umi_fabric_hash64(const void *data,size_t size,uint64_t seed);
/* Compare semantic versions lexicographically. */
int umi_fabric_version_compare(UmiFabricVersion left,UmiFabricVersion right);
/* Return a stable diagnostic label for a fabric state. */
const char *umi_fabric_state_text(UmiFabricState state);

#ifdef __cplusplus
}
#endif
#endif
