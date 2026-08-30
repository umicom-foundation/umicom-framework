/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/rollback_generation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Rollback Generation developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ROLLBACK_GENERATION_H
#define UMICOM_DEVELOPER_CAPABILITIES_ROLLBACK_GENERATION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ROLLBACK_GENERATION "umicom.developer.delivery.rollback_generation"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_rollback_generation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ROLLBACK_GENERATION_H */
