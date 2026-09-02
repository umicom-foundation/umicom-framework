/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/service_key.h
 *
 * PURPOSE:
 *   Create and compare qualified service keys used for explicit dependency lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_SERVICE_KEY_H
#define UMICOM_RUNTIME_BOOTSTRAP_SERVICE_KEY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap service key from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_bootstrap_service_key_init(UmiBootstrapServiceKey *key,
                                         const char *service_id,
                                         const char *qualifier);
/**
 * Provide the bootstrap service key equal operation used by this module and its client
 * applications.
 */
bool umi_bootstrap_service_key_equal(const UmiBootstrapServiceKey *left,
                                     const UmiBootstrapServiceKey *right);
/**
 * Provide the bootstrap service key hash operation used by this module and its client
 * applications.
 */
uint64_t umi_bootstrap_service_key_hash(const UmiBootstrapServiceKey *key);

#ifdef __cplusplus
}
#endif

#endif
