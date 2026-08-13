/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/recovery_journal.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Recovery Journal document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_RECOVERY_JOURNAL_H
#define UMICOM_DOCUMENT_CAPABILITIES_RECOVERY_JOURNAL_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_RECOVERY_JOURNAL "umicom.document.recovery.recovery_journal"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_recovery_journal(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_RECOVERY_JOURNAL_H */
