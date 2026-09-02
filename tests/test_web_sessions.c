/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_sessions.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <string.h>
/*
 * Exercise allow and return a clear result when the behaviour no longer matches its
 * contract.
 */
static int allow(const char *p,const char *perm,void *d){(void)p;(void)d;return strcmp(perm,"read")==0;}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiWebSessionStore *s=NULL;UmiWebOriginPolicy *o=NULL;UmiWebSecurity sec;UmiWebSession session={{0},{0},0U,100U,1};(void)umi_web_copy_text(session.id,sizeof(session.id),"s1");(void)umi_web_copy_text(session.principal,sizeof(session.principal),"sam");assert(umi_web_session_store_create(&s)==UMI_STATUS_OK);assert(umi_web_session_put(s,&session)==UMI_STATUS_OK);assert(umi_web_session_get(s,"s1",1U)!=NULL);assert(umi_web_origin_policy_create(&o)==UMI_STATUS_OK);assert(umi_web_origin_allow(o,"https://local")==UMI_STATUS_OK);assert(umi_web_origin_is_allowed(o,"https://local"));assert(umi_web_security_init(&sec,allow,NULL)==UMI_STATUS_OK);assert(umi_web_security_require(&sec,"sam","read")==UMI_STATUS_OK);assert(umi_web_security_require(&sec,"sam","write")==UMI_STATUS_PERMISSION_DENIED);umi_web_origin_policy_destroy(o);umi_web_session_store_destroy(s);return 0;}
