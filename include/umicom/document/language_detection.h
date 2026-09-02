/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/language_detection.h
 *
 * PURPOSE:
 *   Define reusable filename-to-language/icon/MIME inference for editor hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_LANGUAGE_DETECTION_H
#define UMICOM_DOCUMENT_LANGUAGE_DETECTION_H

#include "umicom/base/status.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the document language identity data shared with callers of this public
 * contract.
 */
typedef struct UmiDocumentLanguageIdentity {
    char language_id[UMI_DOCUMENT_LANGUAGE_CAPACITY];
    char mime_type[UMI_DOCUMENT_MIME_CAPACITY];
    char icon_name[UMI_DOCUMENT_NAME_CAPACITY];
    int text;
} UmiDocumentLanguageIdentity;

/**
 * Provide the document language detect operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_language_detect(
    const char *path_or_name,
    UmiDocumentLanguageIdentity *out_identity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_LANGUAGE_DETECTION_H */
