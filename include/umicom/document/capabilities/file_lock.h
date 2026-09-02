/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/file_lock.h
 *
 * PURPOSE:
 *   Declare the Framework-owned File Lock document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_FILE_LOCK_H
#define UMICOM_DOCUMENT_CAPABILITIES_FILE_LOCK_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_FILE_LOCK "umicom.document.conflict.file_lock"

/**
 * Provide the document capability file lock operation used by this module and its client
 * applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_file_lock(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_FILE_LOCK_H */
