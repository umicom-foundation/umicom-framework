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

/*
 * Initialise dr channel from caller-provided values so later operations receive a known
 * state.
 */
void umi_dr_channel_init(UmiDrChannel *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrChannel){0}; value->kind=UMI_DR_CHANNEL_STABLE; value->stability_rank=4U; } }
/* Check that dr channel satisfies its contract before another service relies on it. */
bool umi_dr_channel_valid(const UmiDrChannel *value) { return value != NULL && (value->id[0] != '\0' && value->kind != 0 && value->stability_rank>0U); }
/*
 * Provide the dr channel fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_channel_fingerprint(const UmiDrChannel *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_channel_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
