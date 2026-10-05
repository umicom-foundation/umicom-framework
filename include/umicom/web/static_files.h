/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/static_files.h
 *
 * PURPOSE:
 *   Serve files from one configured document root without directory traversal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_STATIC_FILES_H
#define UMICOM_WEB_STATIC_FILES_H
#include "umicom/web/response.h"
#include "umicom/web/request.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web static files data shared with callers of this public contract.
 */
typedef struct UmiWebStaticFiles { char root[UMI_WEB_PATH_CAPACITY]; } UmiWebStaticFiles;
/**
 * Initialise web static files from caller-provided values so later operations receive a
 * known state.
 */
/* Capture an explicit absolute root under the shared rooted-file syntax.
 * No CWD lookup or storage access. The copied root remains under 512 bytes;
 * keep the descriptor immutable during serving. Failure preserves it. */
UmiStatus umi_web_static_files_init(UmiWebStaticFiles *files,const char *root);
/**
 * Provide the web static files serve operation used by this module and its client
 * applications.
 */
/* Decode one URL path, never a query. Root and directory requests select
 * index.html. Percent-escaped separators, traversal, repository metadata and
 * ambiguous portable names are refused before storage access. Only regular
 * non-link files under the selected root are read through RootedFileRead.
 * Complete files must fit 16 KiB minus one byte; larger files return HTTP 413,
 * never a successful truncation. Missing files return 404; denied paths 403.
 * I/O belongs on a worker. There is no directory listing, upload or caching. */
UmiStatus umi_web_static_files_serve(const UmiWebStaticFiles *files,const char *request_path,UmiWebResponse *response);
/* Borrow a UmiWebStaticFiles as callback context for RouterSetFallback.
 * Only GET/HEAD are accepted; other methods return 405 with Allow. The closed
 * connection serializer suppresses a HEAD body. API routes retain priority. */
UmiStatus UmiWebStaticFilesHandle(const UmiWebRequest *request,UmiWebResponse *response,void *context);

#ifdef __cplusplus
}
#endif
#endif
