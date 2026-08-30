/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/channel.c
 *
 * PURPOSE:
 *   release channel descriptors and stability ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/channel.h"

void umi_dr_channel_init(UmiDrChannel *value) { if (value != NULL) { *value = (UmiDrChannel){0}; value->kind=UMI_DR_CHANNEL_STABLE; value->stability_rank=4U; } }
bool umi_dr_channel_valid(const UmiDrChannel *value) { return value != NULL && (value->id[0] != '\0' && value->kind != 0 && value->stability_rank>0U); }
uint64_t umi_dr_channel_fingerprint(const UmiDrChannel *value) {
    uint64_t h = 0U;
    if (!umi_dr_channel_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
