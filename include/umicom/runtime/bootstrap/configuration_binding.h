/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/configuration_binding.h
 *
 * PURPOSE:
 *   Convert validated textual configuration into primitive C values.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_BINDING_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_BINDING_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_configuration_bind_bool(const char *value,
                                                bool *out_value);
UmiStatus umi_bootstrap_configuration_bind_i64(const char *value,
                                               int64_t *out_value);

#ifdef __cplusplus
}
#endif

#endif
