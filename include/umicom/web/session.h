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

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_SESSION_H
#define UMICOM_WEB_SESSION_H
#include <stdint.h>
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web session data shared with callers of this public contract.
 */
typedef struct UmiWebSession { char id[96]; char principal[128]; uint64_t created_ns; uint64_t expires_ns; int active; } UmiWebSession;
/**
 * Represent the web session store data shared with callers of this public contract.
 */
typedef struct UmiWebSessionStore UmiWebSessionStore;
/**
 * Initialise web session store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_web_session_store_create(UmiWebSessionStore **out_store);
/**
 * Release or reset state held by web session store so the same storage can be reused
 * safely.
 */
void umi_web_session_store_destroy(UmiWebSessionStore *store);
/**
 * Provide the web session put operation used by this module and its client applications.
 */
UmiStatus umi_web_session_put(UmiWebSessionStore *store,const UmiWebSession *session);
/**
 * Provide the web session get operation used by this module and its client applications.
 */
const UmiWebSession *umi_web_session_get(const UmiWebSessionStore *store,const char *id,uint64_t now_ns);
/**
 * Provide the web session revoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_session_revoke(UmiWebSessionStore *store,const char *id);
/**
 * Return the number of records represented by web session without changing their state.
 */
size_t umi_web_session_count(const UmiWebSessionStore *store);
#ifdef __cplusplus
}
#endif
#endif
