/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/capability_matrix.h
 *
 * PURPOSE:
 *   Cross-reference application features and panels with canonical Framework
 *   capabilities and reusable component coverage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_CAPABILITY_MATRIX_H
#define UMICOM_APPLICATION_PRODUCTISATION_CAPABILITY_MATRIX_H

#include "umicom/application/productisation/types.h"
#include "umicom/runtime/capability_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProductCapabilityUsage {
    char capability_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char category[UMI_PRODUCTISATION_ID_CAPACITY];
    UmiCapabilityMaturity maturity;
    uint64_t application_mask;
    size_t application_count;
    size_t feature_reference_count;
    size_t panel_reference_count;
    size_t component_count;
    int missing_from_framework;
    int shared_across_applications;
} UmiProductCapabilityUsage;

typedef struct UmiProductCapabilityMatrix {
    UmiProductCapabilityUsage usages[UMI_PRODUCTISATION_MAX_CAPABILITIES];
    size_t usage_count;
    size_t shared_count;
    size_t missing_count;
    size_t implemented_count;
    size_t foundation_count;
    size_t planned_count;
    size_t missing_component_count;
} UmiProductCapabilityMatrix;

UmiStatus umi_product_capability_matrix_build(
    UmiProductCapabilityMatrix *out_matrix);
const UmiProductCapabilityUsage *umi_product_capability_matrix_find(
    const UmiProductCapabilityMatrix *matrix,
    const char *capability_id);
int umi_product_capability_usage_has_application(
    const UmiProductCapabilityUsage *usage,
    size_t application_index);

#ifdef __cplusplus
}
#endif

#endif
