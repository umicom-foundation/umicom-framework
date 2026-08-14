/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/local_provider.h
 *
 * PURPOSE:
 *   Expose the Framework local filesystem provider through the generic ABI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_LOCAL_PROVIDER_H
#define UMICOM_DOCUMENT_LOCAL_PROVIDER_H

#include "umicom/document/provider.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiDocumentProvider umi_document_local_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_LOCAL_PROVIDER_H */
