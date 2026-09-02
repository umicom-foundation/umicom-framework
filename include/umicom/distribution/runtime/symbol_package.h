/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/symbol_package.h
 *
 * PURPOSE:
 *   debug symbol package metadata and build-id matching.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_SYMBOL_PACKAGE_H
#define UMICOM_DISTRIBUTION_RUNTIME_SYMBOL_PACKAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr symbol package data shared with callers of this public contract.
 */
typedef struct UmiDrSymbolPackage { char id[UMI_DR_ID_CAPACITY]; char build_id[UMI_DR_DIGEST_CAPACITY]; char digest[UMI_DR_DIGEST_CAPACITY]; uint64_t size_bytes; } UmiDrSymbolPackage;
/**
 * Initialise dr symbol package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_symbol_package_init(UmiDrSymbolPackage *value);
/**
 * Check that dr symbol package satisfies its contract before another service relies on it.
 */
bool umi_dr_symbol_package_valid(const UmiDrSymbolPackage *value);
/**
 * Provide the dr symbol package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_symbol_package_fingerprint(const UmiDrSymbolPackage *value);

#ifdef __cplusplus
}
#endif
#endif
