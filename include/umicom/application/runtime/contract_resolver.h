/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/contract_resolver.h
 *
 * PURPOSE:
 *   Resolve canonical application definition and product experience as one thin-client contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_CONTRACT_RESOLVER_H
#define UMICOM_APPLICATION_RUNTIME_CONTRACT_RESOLVER_H

#include "umicom/application/definition.h"
#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationContract {
    uint32_t structure_size;
    const UmiApplicationDefinition *definition;
    const UmiApplicationExperienceDefinition *experience;
} UmiApplicationContract;

UmiStatus umi_application_contract_resolve(
    const char *application_id,
    UmiApplicationContract *out_contract);
UmiStatus umi_application_contract_validate(
    const UmiApplicationContract *contract);

#ifdef __cplusplus
}
#endif

#endif
