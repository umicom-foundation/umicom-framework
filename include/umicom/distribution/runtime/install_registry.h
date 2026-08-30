/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/install_registry.h
 *
 * PURPOSE:
 *   bounded registry of installed applications and versions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_INSTALL_REGISTRY_H
#define UMICOM_DISTRIBUTION_RUNTIME_INSTALL_REGISTRY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/distribution/runtime/install_state.h"

typedef struct UmiDrInstallRegistry { UmiDrInstallState items[UMI_DR_MAX_ITEMS]; size_t count; } UmiDrInstallRegistry;
void umi_dr_install_registry_init(UmiDrInstallRegistry *catalogue);
UmiStatus umi_dr_install_registry_add(UmiDrInstallRegistry *catalogue, const UmiDrInstallState *item);
const UmiDrInstallState *umi_dr_install_registry_find(const UmiDrInstallRegistry *catalogue, const char *id);

#ifdef __cplusplus
}
#endif
#endif
