/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/file_identity.h
 *
 * PURPOSE:
 *   Define native file metadata and external-change comparison services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_FILE_IDENTITY_H
#define UMICOM_DOCUMENT_FILE_IDENTITY_H

#include "umicom/base/status.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_document_file_info(const char *path,
                                 UmiDocumentFileInfo *out_info);
UmiStatus umi_document_file_changed(const char *path,
                                    const UmiDocumentFingerprint *baseline,
                                    int *out_changed,
                                    UmiDocumentFingerprint *out_current);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_FILE_IDENTITY_H */
