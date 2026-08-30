/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/session.h
 *
 * PURPOSE:
 *   Manage revocable in-memory web sessions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_SESSION_H
#define UMICOM_WEB_SESSION_H
#include <stdint.h>
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebSession { char id[96]; char principal[128]; uint64_t created_ns; uint64_t expires_ns; int active; } UmiWebSession;
typedef struct UmiWebSessionStore UmiWebSessionStore;
UmiStatus umi_web_session_store_create(UmiWebSessionStore **out_store);
void umi_web_session_store_destroy(UmiWebSessionStore *store);
UmiStatus umi_web_session_put(UmiWebSessionStore *store,const UmiWebSession *session);
const UmiWebSession *umi_web_session_get(const UmiWebSessionStore *store,const char *id,uint64_t now_ns);
UmiStatus umi_web_session_revoke(UmiWebSessionStore *store,const char *id);
size_t umi_web_session_count(const UmiWebSessionStore *store);
#ifdef __cplusplus
}
#endif
#endif
