/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/contract.h
 *
 * PURPOSE:
 *   Derive and validate versioned component contracts from the canonical
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_CONTRACT_H
#define UMICOM_APPLICATION_COMPONENT_CONTRACT_H

#include "umicom/application/component/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application component contract from definition operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_component_contract_from_definition(
    const UmiApplicationComponentDefinition *definition,
    UmiApplicationComponentContract *out_contract);
/**
 * Check that application component contract satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_component_contract_validate(
    const UmiApplicationComponentContract *contract);
/**
 * Provide the application component contract supports operation used by this module and
 * its client applications.
 */
int umi_application_component_contract_supports(
    const UmiApplicationComponentContract *contract,
    UmiApplicationComponentFrontend frontend);
/**
 * Provide the application component version compatible operation used by this module and
 * its client applications.
 */
int umi_application_component_version_compatible(
    UmiApplicationComponentVersion available,
    UmiApplicationComponentVersion required);

#ifdef __cplusplus
}
#endif

#endif
