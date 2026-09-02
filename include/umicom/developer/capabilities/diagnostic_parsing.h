/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/diagnostic_parsing.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Diagnostic Parsing developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_DIAGNOSTIC_PARSING_H
#define UMICOM_DEVELOPER_CAPABILITIES_DIAGNOSTIC_PARSING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_DIAGNOSTIC_PARSING "umicom.developer.build.diagnostic_parsing"

/**
 * Provide the developer capability diagnostic parsing operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_diagnostic_parsing(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_DIAGNOSTIC_PARSING_H */
