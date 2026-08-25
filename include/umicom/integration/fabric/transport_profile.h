/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/transport_profile.h
 *
 * PURPOSE:
 *   Describe bounded transport limits, heartbeat policy and security requirements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_TRANSPORT_PROFILE_H
#define UMICOM_INTEGRATION_FABRIC_TRANSPORT_PROFILE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricTransportProfile {
    char profile_id[UMI_FABRIC_ID_CAPACITY];
    uint64_t max_frame_bytes;
    uint32_t heartbeat_ms;
    uint32_t idle_timeout_ms;
    bool compression_allowed;
    bool tls_required;
} UmiFabricTransportProfile;

UmiStatus umi_fabric_transport_profile_init(UmiFabricTransportProfile *item, const char *profile_id, uint64_t max_frame_bytes, uint32_t heartbeat_ms, uint32_t idle_timeout_ms, bool compression_allowed, bool tls_required);
UmiStatus umi_fabric_transport_profile_validate(const UmiFabricTransportProfile *item);

#ifdef __cplusplus
}
#endif
#endif
