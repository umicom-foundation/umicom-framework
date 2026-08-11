/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/runtime_bundle.h
 *
 * PURPOSE:
 *   Describe runtime files that must travel with an application package.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The runtime bundle lets packaging code distinguish the main executable from required DLLs, data files and resources.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_RUNTIME_BUNDLE_H
#define INCLUDE_UMICOM_DELIVERY_RUNTIME_BUNDLE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRuntimeBundle {
    char files[UMI_DELIVERY_MAX_ARTIFACTS][UMI_DELIVERY_PATH_CAPACITY];
    size_t count;
} UmiRuntimeBundle;
void umi_runtime_bundle_init(UmiRuntimeBundle *bundle);
UmiStatus umi_runtime_bundle_add(UmiRuntimeBundle *bundle,
                                 const char *path);
int umi_runtime_bundle_contains(const UmiRuntimeBundle *bundle,
                                const char *path);

#ifdef __cplusplus
}
#endif

#endif
