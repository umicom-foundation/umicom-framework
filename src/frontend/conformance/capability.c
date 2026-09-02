/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/capability.c
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
#include "umicom/frontend/conformance/capability.h"

/* Provide the fc capability bit operation used by this module and its client applications. */
uint64_t umi_fc_capability_bit(UmiFcCapabilityKind kind) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((unsigned)kind >= (unsigned)UMI_FC_CAP_COUNT) return 0U;
    return UINT64_C(1) << (unsigned)kind;
}
/*
 * Provide the fc capability make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_capability_make(UmiFcCapabilityKind kind, const char *id, const char *label, bool essential, UmiFcCapability *out_capability) {
    UmiStatus st;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_capability == NULL || umi_fc_capability_bit(kind) == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    st = umi_fc_copy_text(out_capability->id, sizeof(out_capability->id), id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (st != UMI_STATUS_OK) return st;
    st = umi_fc_copy_text(out_capability->label, sizeof(out_capability->label), label); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (st != UMI_STATUS_OK) return st;
    out_capability->kind = kind; out_capability->essential = essential; return UMI_STATUS_OK;
}
