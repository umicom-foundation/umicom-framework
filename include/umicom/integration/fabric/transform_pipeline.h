/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/transform_pipeline.h
 *
 * PURPOSE:
 *   Validate transform-plan completeness and calculate a deterministic pipeline fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_TRANSFORM_PIPELINE_H
#define UMICOM_INTEGRATION_FABRIC_TRANSFORM_PIPELINE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/transform_plan.h"
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Check that fabric transform pipeline satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_transform_pipeline_validate(const UmiFabricTransformPlan *plan,size_t maximum_steps,uint64_t *out_fingerprint);

#ifdef __cplusplus
}
#endif
#endif
