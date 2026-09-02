/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/starter_catalogue.h
 *
 * PURPOSE:
 *   Maintain Framework-owned application starter definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_STARTER_CATALOGUE_H
#define UMICOM_RUNTIME_BOOTSTRAP_STARTER_CATALOGUE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap starter catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_starter_catalogue_init(UmiBootstrapStarterCatalogue *catalogue);
/**
 * Add bootstrap starter catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_starter_catalogue_add(
    UmiBootstrapStarterCatalogue *catalogue,
    const UmiBootstrapStarterDescriptor *starter);
/**
 * Find bootstrap starter catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiBootstrapStarterDescriptor *umi_bootstrap_starter_catalogue_find(
    const UmiBootstrapStarterCatalogue *catalogue,
    const char *starter_id);

#ifdef __cplusplus
}
#endif

#endif
