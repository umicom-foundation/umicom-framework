/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/optional_binding.h
 *
 * PURPOSE:
 *   Resolve optional dependency presence and fallback decisions explicitly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_OPTIONAL_BINDING_H
#define UMICOM_RUNTIME_BOOTSTRAP_OPTIONAL_BINDING_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void *umi_bootstrap_optional_binding_choose(void *resolved_instance,
                                            void *fallback_instance);

#ifdef __cplusplus
}
#endif

#endif
