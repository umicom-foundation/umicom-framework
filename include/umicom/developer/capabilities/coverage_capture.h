/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/coverage_capture.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Coverage Capture developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_COVERAGE_CAPTURE_H
#define UMICOM_DEVELOPER_CAPABILITIES_COVERAGE_CAPTURE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_COVERAGE_CAPTURE "umicom.developer.testing.coverage_capture"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_coverage_capture(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_COVERAGE_CAPTURE_H */
