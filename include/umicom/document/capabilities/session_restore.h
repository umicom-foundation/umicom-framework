/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/session_restore.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Session Restore document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_SESSION_RESTORE_H
#define UMICOM_DOCUMENT_CAPABILITIES_SESSION_RESTORE_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_SESSION_RESTORE "umicom.document.recovery.session_restore"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_session_restore(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_SESSION_RESTORE_H */
