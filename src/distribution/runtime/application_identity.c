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

void umi_dr_application_identity_init(UmiDrApplicationIdentity *value) { if (value != NULL) { *value = (UmiDrApplicationIdentity){0};  } }
bool umi_dr_application_identity_valid(const UmiDrApplicationIdentity *value) { return value != NULL && (value->id[0] != '\0' && value->publisher[0] != '\0' && value->product[0] != '\0'); }
uint64_t umi_dr_application_identity_fingerprint(const UmiDrApplicationIdentity *value) {
    uint64_t h = 0U;
    if (!umi_dr_application_identity_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
