/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/progress_reporting.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Progress Reporting developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PROGRESS_REPORTING_H
#define UMICOM_DEVELOPER_CAPABILITIES_PROGRESS_REPORTING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PROGRESS_REPORTING "umicom.developer.diagnostics.progress_reporting"

/**
 * Provide the developer capability progress reporting operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_progress_reporting(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PROGRESS_REPORTING_H */
