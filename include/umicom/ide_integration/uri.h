/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/uri.h
 *
 * PURPOSE:
 *   Convert common local file URIs into paths for DocumentCoordinator navigation
 *   while preserving non-file URIs for services that understand them directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_URI_H
#define UMICOM_IDE_INTEGRATION_URI_H
#include "umicom/ide_integration/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ide uri to local path operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_uri_to_local_path(
    const char *uri,
    char *out_path,
    size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
