/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/model_registry.h
 *
 * PURPOSE:
 *   Register bounded named quantitative models with deterministic priorities.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_MODEL_REGISTRY_H
#define INCLUDE_UMICOM_FINANCE_QUANT_MODEL_REGISTRY_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant model descriptor data shared with callers of this public contract.
 */
typedef struct UmiQuantModelDescriptor { char name[UMI_QUANT_NAME_CAPACITY]; UmiQuantModelKind kind; int32_t priority; int enabled; } UmiQuantModelDescriptor;
/**
 * Represent the quant model registry data shared with callers of this public contract.
 */
typedef struct UmiQuantModelRegistry { UmiQuantModelDescriptor models[UMI_QUANT_MAX_MODELS]; size_t count; } UmiQuantModelRegistry;
/* Initialise an empty model registry. */
void umi_quant_model_registry_init(UmiQuantModelRegistry *registry);
/* Add one uniquely named model. */
UmiStatus umi_quant_model_registry_add(UmiQuantModelRegistry *registry, const char *name, UmiQuantModelKind kind, int32_t priority, int enabled);
/* Select the highest-priority enabled model of a requested kind. */
UmiStatus umi_quant_model_registry_select(const UmiQuantModelRegistry *registry, UmiQuantModelKind kind, const UmiQuantModelDescriptor **out_model);

#ifdef __cplusplus
}
#endif

#endif
