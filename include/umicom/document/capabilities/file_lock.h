/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/file_lock.h
 *
 * PURPOSE:
 *   Declare the Framework-owned File Lock document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_FILE_LOCK_H
#define UMICOM_DOCUMENT_CAPABILITIES_FILE_LOCK_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_FILE_LOCK "umicom.document.conflict.file_lock"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_file_lock(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_FILE_LOCK_H */
