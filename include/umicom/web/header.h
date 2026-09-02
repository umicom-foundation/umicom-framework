/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/header.h
 *
 * PURPOSE:
 *   Represent one bounded HTTP header and provide case-insensitive header-name matching.
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

#ifndef UMICOM_WEB_HEADER_H
#define UMICOM_WEB_HEADER_H
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web header data shared with callers of this public contract.
 */
typedef struct UmiWebHeader { char name[UMI_WEB_HEADER_NAME_CAPACITY]; char value[UMI_WEB_HEADER_VALUE_CAPACITY]; } UmiWebHeader;
/**
 * Copy web header into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_web_header_set(UmiWebHeader *header,const char *name,const char *value);
/**
 * Provide the web header name equal operation used by this module and its client
 * applications.
 */
int umi_web_header_name_equal(const UmiWebHeader *header,const char *name);
#ifdef __cplusplus
}
#endif
#endif
