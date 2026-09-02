/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/file_identity.h
 *
 * PURPOSE:
 *   Define native file metadata and external-change comparison services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_FILE_IDENTITY_H
#define UMICOM_DOCUMENT_FILE_IDENTITY_H

#include "umicom/base/status.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the document file info operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_file_info(const char *path,
                                 UmiDocumentFileInfo *out_info);
/**
 * Provide the document file changed operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_file_changed(const char *path,
                                    const UmiDocumentFingerprint *baseline,
                                    int *out_changed,
                                    UmiDocumentFingerprint *out_current);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_FILE_IDENTITY_H */
