/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/collection_binding.h
 *
 * PURPOSE:
 *   Build ordered collections of service identifiers for multi-bind dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_COLLECTION_BINDING_H
#define UMICOM_RUNTIME_BOOTSTRAP_COLLECTION_BINDING_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_collection_binding_init(UmiBootstrapIdList *collection);
UmiStatus umi_bootstrap_collection_binding_add(UmiBootstrapIdList *collection,
                                               const char *service_id);

#ifdef __cplusplus
}
#endif

#endif
