/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/loader.h
 *
 * PURPOSE:
 *   Define secure provider-neutral loading with encoding and line-ending analysis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_LOADER_H
#define UMICOM_DOCUMENT_LOADER_H

#include "umicom/document/provider.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the document load options default operation used by this module and its client
 * applications.
 */
UmiDocumentLoadOptions umi_document_load_options_default(void);
/**
 * Read document into validated module state and return a status when input cannot be used.
 */
UmiStatus umi_document_load(const UmiDocumentProvider *provider,
                            const char *resource,
                            const UmiDocumentLoadOptions *options,
                            UmiDocumentLoadResult *out_result);
/**
 * Release or reset state held by document load result so the same storage can be reused
 * safely.
 */
void umi_document_load_result_dispose(UmiDocumentLoadResult *result);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_LOADER_H */
