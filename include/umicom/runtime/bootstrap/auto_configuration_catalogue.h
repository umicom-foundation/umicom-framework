/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/auto_configuration_catalogue.h
 *
 * PURPOSE:
 *   Maintain a deterministic catalogue of auto-configuration definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_CATALOGUE_H
#define UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_CATALOGUE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_auto_configuration_catalogue_init(
    UmiBootstrapAutoConfigurationCatalogue *catalogue);
UmiStatus umi_bootstrap_auto_configuration_catalogue_add(
    UmiBootstrapAutoConfigurationCatalogue *catalogue,
    const UmiBootstrapAutoConfiguration *configuration);
const UmiBootstrapAutoConfiguration *umi_bootstrap_auto_configuration_catalogue_find(
    const UmiBootstrapAutoConfigurationCatalogue *catalogue,
    const char *configuration_id);

#ifdef __cplusplus
}
#endif

#endif
