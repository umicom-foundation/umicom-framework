/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/recovery_journal.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Recovery Journal document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_RECOVERY_JOURNAL_H
#define UMICOM_DOCUMENT_CAPABILITIES_RECOVERY_JOURNAL_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_RECOVERY_JOURNAL "umicom.document.recovery.recovery_journal"

/**
 * Provide the document capability recovery journal operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_recovery_journal(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_RECOVERY_JOURNAL_H */
