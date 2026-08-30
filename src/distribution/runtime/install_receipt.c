/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/install_receipt.c
 *
 * PURPOSE:
 *   immutable installation receipt and package fingerprint evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/install_receipt.h"

void umi_dr_install_receipt_init(UmiDrInstallReceipt *value) { if (value != NULL) { *value = (UmiDrInstallReceipt){0};  } }
bool umi_dr_install_receipt_valid(const UmiDrInstallReceipt *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->package_digest[0] != '\0' && value->installed_at>0U); }
uint64_t umi_dr_install_receipt_fingerprint(const UmiDrInstallReceipt *value) {
    uint64_t h = 0U;
    if (!umi_dr_install_receipt_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
