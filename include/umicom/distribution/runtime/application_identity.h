/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/application_identity.h
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
#ifndef UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_IDENTITY_H
#define UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_IDENTITY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr application identity data shared with callers of this public contract.
 */
typedef struct UmiDrApplicationIdentity { char id[UMI_DR_ID_CAPACITY]; char publisher[UMI_DR_TEXT_CAPACITY]; char family[UMI_DR_ID_CAPACITY]; char product[UMI_DR_TEXT_CAPACITY]; } UmiDrApplicationIdentity;
/**
 * Initialise dr application identity from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_application_identity_init(UmiDrApplicationIdentity *value);
/**
 * Check that dr application identity satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_application_identity_valid(const UmiDrApplicationIdentity *value);
/**
 * Provide the dr application identity fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_identity_fingerprint(const UmiDrApplicationIdentity *value);

#ifdef __cplusplus
}
#endif
#endif
