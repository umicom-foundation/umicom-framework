/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/uri.h
 *
 * PURPOSE:
 *   Convert common local file URIs into paths for DocumentCoordinator navigation
 *   while preserving non-file URIs for services that understand them directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_URI_H
#define UMICOM_IDE_INTEGRATION_URI_H
#include "umicom/ide_integration/types.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ide_uri_to_local_path(
    const char *uri,
    char *out_path,
    size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
