/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/parallel_build.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Parallel Build developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PARALLEL_BUILD_H
#define UMICOM_DEVELOPER_CAPABILITIES_PARALLEL_BUILD_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PARALLEL_BUILD "umicom.developer.build.parallel_build"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_parallel_build(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PARALLEL_BUILD_H */
