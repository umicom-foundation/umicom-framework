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

UmiStatus umi_application_component_contract_from_definition(
    const UmiApplicationComponentDefinition *definition,
    UmiApplicationComponentContract *out_contract);
UmiStatus umi_application_component_contract_validate(
    const UmiApplicationComponentContract *contract);
int umi_application_component_contract_supports(
    const UmiApplicationComponentContract *contract,
    UmiApplicationComponentFrontend frontend);
int umi_application_component_version_compatible(
    UmiApplicationComponentVersion available,
    UmiApplicationComponentVersion required);

#ifdef __cplusplus
}
#endif

#endif
