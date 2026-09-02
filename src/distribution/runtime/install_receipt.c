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

/*
 * Initialise dr install receipt from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_install_receipt_init(UmiDrInstallReceipt *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrInstallReceipt){0};  } }
/*
 * Check that dr install receipt satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_install_receipt_valid(const UmiDrInstallReceipt *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->package_digest[0] != '\0' && value->installed_at>0U); }
/*
 * Provide the dr install receipt fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_install_receipt_fingerprint(const UmiDrInstallReceipt *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_install_receipt_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
