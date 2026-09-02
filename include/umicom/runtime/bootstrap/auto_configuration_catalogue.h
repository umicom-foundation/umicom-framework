/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/auto_configuration_catalogue.h
 *
 * PURPOSE:
 *   Maintain a deterministic catalogue of auto-configuration definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_CATALOGUE_H
#define UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_CATALOGUE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap auto configuration catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_bootstrap_auto_configuration_catalogue_init(
    UmiBootstrapAutoConfigurationCatalogue *catalogue);
/**
 * Add bootstrap auto configuration catalogue only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_bootstrap_auto_configuration_catalogue_add(
    UmiBootstrapAutoConfigurationCatalogue *catalogue,
    const UmiBootstrapAutoConfiguration *configuration);
/**
 * Find bootstrap auto configuration catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiBootstrapAutoConfiguration *umi_bootstrap_auto_configuration_catalogue_find(
    const UmiBootstrapAutoConfigurationCatalogue *catalogue,
    const char *configuration_id);

#ifdef __cplusplus
}
#endif

#endif
