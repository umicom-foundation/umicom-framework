/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/starter_catalogue.h
 *
 * PURPOSE:
 *   Maintain Framework-owned application starter definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_STARTER_CATALOGUE_H
#define UMICOM_RUNTIME_BOOTSTRAP_STARTER_CATALOGUE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_starter_catalogue_init(UmiBootstrapStarterCatalogue *catalogue);
UmiStatus umi_bootstrap_starter_catalogue_add(
    UmiBootstrapStarterCatalogue *catalogue,
    const UmiBootstrapStarterDescriptor *starter);
const UmiBootstrapStarterDescriptor *umi_bootstrap_starter_catalogue_find(
    const UmiBootstrapStarterCatalogue *catalogue,
    const char *starter_id);

#ifdef __cplusplus
}
#endif

#endif
