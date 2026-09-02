/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/configuration_binding.h
 *
 * PURPOSE:
 *   Convert validated textual configuration into primitive C values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_BINDING_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_BINDING_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap configuration bind bool operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_configuration_bind_bool(const char *value,
                                                bool *out_value);
/**
 * Provide the bootstrap configuration bind i64 operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_configuration_bind_i64(const char *value,
                                               int64_t *out_value);

#ifdef __cplusplus
}
#endif

#endif
