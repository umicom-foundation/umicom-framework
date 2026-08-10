/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/capability_catalogue.h
 *
 * PURPOSE:
 *   Publish one canonical catalogue of reusable Framework capabilities and
 *   maturity states so Studio, Designer, Trader, TMS, scaffolding, documentation,
 *   and native tools use the same identifiers instead of private duplicates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_CAPABILITY_CATALOGUE_H
#define UMICOM_RUNTIME_CAPABILITY_CATALOGUE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiCapabilityMaturity {
    UMI_CAPABILITY_IMPLEMENTED = 1,
    UMI_CAPABILITY_FOUNDATION = 2,
    UMI_CAPABILITY_PLANNED = 3
} UmiCapabilityMaturity;

typedef struct UmiFrameworkCapabilityDefinition {
    const char *capability_id;
    const char *category;
    UmiCapabilityMaturity maturity;
    const char *description;
} UmiFrameworkCapabilityDefinition;

size_t umi_framework_capability_catalogue_count(void);
const UmiFrameworkCapabilityDefinition *umi_framework_capability_catalogue_at(
    size_t index
);
const UmiFrameworkCapabilityDefinition *umi_framework_capability_catalogue_find(
    const char *capability_id
);
const char *umi_capability_maturity_text(UmiCapabilityMaturity maturity);

#ifdef __cplusplus
}
#endif

#endif
