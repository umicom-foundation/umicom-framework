/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/runtime_bundle.h
 *
 * PURPOSE:
 *   Describe runtime files that must travel with an application package.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The runtime bundle lets packaging code distinguish the main executable from required DLLs, data files and resources.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_RUNTIME_BUNDLE_H
#define INCLUDE_UMICOM_DELIVERY_RUNTIME_BUNDLE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the runtime bundle data shared with callers of this public contract.
 */
typedef struct UmiRuntimeBundle {
    char files[UMI_DELIVERY_MAX_ARTIFACTS][UMI_DELIVERY_PATH_CAPACITY];
    size_t count;
} UmiRuntimeBundle;
/**
 * Initialise runtime bundle from caller-provided values so later operations receive a
 * known state.
 */
void umi_runtime_bundle_init(UmiRuntimeBundle *bundle);
/**
 * Add runtime bundle only after its inputs and available capacity have been checked.
 */
UmiStatus umi_runtime_bundle_add(UmiRuntimeBundle *bundle,
                                 const char *path);
/**
 * Provide the runtime bundle contains operation used by this module and its client
 * applications.
 */
int umi_runtime_bundle_contains(const UmiRuntimeBundle *bundle,
                                const char *path);

#ifdef __cplusplus
}
#endif

#endif
