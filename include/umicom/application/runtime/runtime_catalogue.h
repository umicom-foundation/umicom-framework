/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/runtime_catalogue.h
 *
 * PURPOSE:
 *   Validate catalogue alignment and expose cross-product readiness summaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_RUNTIME_CATALOGUE_H
#define UMICOM_APPLICATION_RUNTIME_RUNTIME_CATALOGUE_H

#include "umicom/application/runtime/readiness.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationRuntimeCatalogueStatus {
    uint32_t structure_size;
    size_t portfolio_count;
    size_t experience_count;
    size_t aligned_count;
    size_t missing_experience_count;
    unsigned average_readiness_percent;
} UmiApplicationRuntimeCatalogueStatus;

UmiStatus umi_application_runtime_catalogue_status(
    UmiApplicationRuntimeCatalogueStatus *out_status);
UmiStatus umi_application_runtime_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
