/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/application_identity.c
 *
 * PURPOSE:
 *   stable application identity, publisher and product-family metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_identity.h"

/*
 * Initialise dr application identity from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_application_identity_init(UmiDrApplicationIdentity *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrApplicationIdentity){0};  } }
/*
 * Check that dr application identity satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_application_identity_valid(const UmiDrApplicationIdentity *value) { return value != NULL && (value->id[0] != '\0' && value->publisher[0] != '\0' && value->product[0] != '\0'); }
/*
 * Provide the dr application identity fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_identity_fingerprint(const UmiDrApplicationIdentity *value) {
    uint64_t h = 0U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_dr_application_identity_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
