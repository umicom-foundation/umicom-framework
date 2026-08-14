/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/loader.h
 *
 * PURPOSE:
 *   Define secure provider-neutral loading with encoding and line-ending analysis.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_LOADER_H
#define UMICOM_DOCUMENT_LOADER_H

#include "umicom/document/provider.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiDocumentLoadOptions umi_document_load_options_default(void);
UmiStatus umi_document_load(const UmiDocumentProvider *provider,
                            const char *resource,
                            const UmiDocumentLoadOptions *options,
                            UmiDocumentLoadResult *out_result);
void umi_document_load_result_dispose(UmiDocumentLoadResult *result);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_LOADER_H */
