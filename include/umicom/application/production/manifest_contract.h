/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/manifest_contract.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_MANIFEST_CONTRACT_H
#define UMICOM_APPLICATION_PRODUCTION_MANIFEST_CONTRACT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"

typedef struct UmiApplicationProductionManifestContract {
    char application_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    char display_name[UMI_APPLICATION_PRODUCTION_TEXT_CAPACITY];
    char executable_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    char default_layout_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    uint32_t frontend_flags;
    size_t panel_count;
    size_t layout_count;
    size_t feature_count;
} UmiApplicationProductionManifestContract;

UmiStatus umi_application_production_manifest_contract_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionManifestContract *out_contract);
UmiStatus umi_application_production_manifest_contract_validate(
    const UmiApplicationProductionManifestContract *contract);

#ifdef __cplusplus
}
#endif
#endif
