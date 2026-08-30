/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/capability.h
 *
 * PURPOSE:
 *   individual semantic frontend capability descriptors and capability bit mapping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CAPABILITY_H
#define UMICOM_FRONTEND_CONFORMANCE_CAPABILITY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcCapability { UmiFcCapabilityKind kind; char id[UMI_FC_ID_CAPACITY]; char label[UMI_FC_TEXT_CAPACITY]; bool essential; } UmiFcCapability;
uint64_t umi_fc_capability_bit(UmiFcCapabilityKind kind);
UmiStatus umi_fc_capability_make(UmiFcCapabilityKind kind, const char *id, const char *label, bool essential, UmiFcCapability *out_capability);

#ifdef __cplusplus
}
#endif
#endif
