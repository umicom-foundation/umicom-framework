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
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebStaticFiles { char root[UMI_WEB_PATH_CAPACITY]; } UmiWebStaticFiles;
UmiStatus umi_web_static_files_init(UmiWebStaticFiles *files,const char *root);
UmiStatus umi_web_static_files_serve(const UmiWebStaticFiles *files,const char *request_path,UmiWebResponse *response);
#ifdef __cplusplus
}
#endif
#endif
