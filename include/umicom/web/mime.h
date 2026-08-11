/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/mime.h
 *
 * PURPOSE:
 *   Map common file extensions to Internet media types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_MIME_H
#define UMICOM_WEB_MIME_H
#ifdef __cplusplus
extern "C" {
#endif
const char *umi_web_mime_from_path(const char *path);
#ifdef __cplusplus
}
#endif
#endif
