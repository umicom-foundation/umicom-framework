/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/structured_logging.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Structured Logging developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_STRUCTURED_LOGGING_H
#define UMICOM_DEVELOPER_CAPABILITIES_STRUCTURED_LOGGING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_STRUCTURED_LOGGING "umicom.developer.diagnostics.structured_logging"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_structured_logging(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_STRUCTURED_LOGGING_H */
