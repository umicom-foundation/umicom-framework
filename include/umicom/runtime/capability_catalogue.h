/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/capability_catalogue.h
 *
 * PURPOSE:
 *   Publish one canonical catalogue of reusable Framework capabilities and
 *   maturity states so Studio, Designer, Trader, TMS, scaffolding, documentation,
 *   and native tools use the same identifiers instead of private duplicates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_CAPABILITY_CATALOGUE_H
#define UMICOM_RUNTIME_CAPABILITY_CATALOGUE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named capability maturity values accepted by this public contract.
 */
typedef enum UmiCapabilityMaturity {
    UMI_CAPABILITY_IMPLEMENTED = 1,
    UMI_CAPABILITY_FOUNDATION = 2,
    UMI_CAPABILITY_PLANNED = 3
} UmiCapabilityMaturity;

/**
 * Represent the framework capability definition data shared with callers of this public
 * contract.
 */
typedef struct UmiFrameworkCapabilityDefinition {
    const char *capability_id;
    const char *category;
    UmiCapabilityMaturity maturity;
    const char *description;
} UmiFrameworkCapabilityDefinition;

/**
 * Return the number of records represented by framework capability catalogue without
 * changing their state.
 */
size_t umi_framework_capability_catalogue_count(void);
/**
 * Find framework capability catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiFrameworkCapabilityDefinition *umi_framework_capability_catalogue_at(
    size_t index
);
/**
 * Find framework capability catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiFrameworkCapabilityDefinition *umi_framework_capability_catalogue_find(
    const char *capability_id
);
/**
 * Provide the capability maturity text operation used by this module and its client
 * applications.
 */
const char *umi_capability_maturity_text(UmiCapabilityMaturity maturity);

#ifdef __cplusplus
}
#endif

#endif
