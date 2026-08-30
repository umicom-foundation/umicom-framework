/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/generation_promotion.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Generation Promotion developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_GENERATION_PROMOTION_H
#define UMICOM_DEVELOPER_CAPABILITIES_GENERATION_PROMOTION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_GENERATION_PROMOTION "umicom.developer.delivery.generation_promotion"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_generation_promotion(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_GENERATION_PROMOTION_H */
