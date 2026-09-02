/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/crash_recovery.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Crash Recovery document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_CRASH_RECOVERY_H
#define UMICOM_DOCUMENT_CAPABILITIES_CRASH_RECOVERY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_CRASH_RECOVERY "umicom.document.recovery.crash_recovery"

/**
 * Provide the document capability crash recovery operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_crash_recovery(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_CRASH_RECOVERY_H */
