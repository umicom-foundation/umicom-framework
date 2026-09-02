/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/install_receipt.h
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
#ifndef UMICOM_DISTRIBUTION_RUNTIME_INSTALL_RECEIPT_H
#define UMICOM_DISTRIBUTION_RUNTIME_INSTALL_RECEIPT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr install receipt data shared with callers of this public contract.
 */
typedef struct UmiDrInstallReceipt { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; char package_digest[UMI_DR_DIGEST_CAPACITY]; UmiDrVersion version; uint64_t installed_at; } UmiDrInstallReceipt;
/**
 * Initialise dr install receipt from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_install_receipt_init(UmiDrInstallReceipt *value);
/**
 * Check that dr install receipt satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_install_receipt_valid(const UmiDrInstallReceipt *value);
/**
 * Provide the dr install receipt fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_install_receipt_fingerprint(const UmiDrInstallReceipt *value);

#ifdef __cplusplus
}
#endif
#endif
