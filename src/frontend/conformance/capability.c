/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/capability.c
 *
 * PURPOSE:
 *   individual semantic frontend capability descriptors and capability bit mapping.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/capability.h"

uint64_t umi_fc_capability_bit(UmiFcCapabilityKind kind) {
    if ((unsigned)kind >= (unsigned)UMI_FC_CAP_COUNT) return 0U;
    return UINT64_C(1) << (unsigned)kind;
}
UmiStatus umi_fc_capability_make(UmiFcCapabilityKind kind, const char *id, const char *label, bool essential, UmiFcCapability *out_capability) {
    UmiStatus st;
    if (out_capability == NULL || umi_fc_capability_bit(kind) == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    st = umi_fc_copy_text(out_capability->id, sizeof(out_capability->id), id); if (st != UMI_STATUS_OK) return st;
    st = umi_fc_copy_text(out_capability->label, sizeof(out_capability->label), label); if (st != UMI_STATUS_OK) return st;
    out_capability->kind = kind; out_capability->essential = essential; return UMI_STATUS_OK;
}
