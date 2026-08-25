/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/install_receipt.h
 *
 * PURPOSE:
 *   immutable installation receipt and package fingerprint evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDrInstallReceipt { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; char package_digest[UMI_DR_DIGEST_CAPACITY]; UmiDrVersion version; uint64_t installed_at; } UmiDrInstallReceipt;
void umi_dr_install_receipt_init(UmiDrInstallReceipt *value);
bool umi_dr_install_receipt_valid(const UmiDrInstallReceipt *value);
uint64_t umi_dr_install_receipt_fingerprint(const UmiDrInstallReceipt *value);

#ifdef __cplusplus
}
#endif
#endif
