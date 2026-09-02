/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/file_size_limit.h
 *
 * PURPOSE:
 *   Declare the Framework-owned File Size Limit document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_FILE_SIZE_LIMIT_H
#define UMICOM_DOCUMENT_CAPABILITIES_FILE_SIZE_LIMIT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_FILE_SIZE_LIMIT "umicom.document.security.file_size_limit"

/**
 * Provide the document capability file size limit operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_file_size_limit(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_FILE_SIZE_LIMIT_H */
