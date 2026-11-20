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

/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include "umicom/umicom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
/*
 * Exercise allow and return a clear result when the behaviour no longer matches its
 * contract.
 */
static int allow(const char *p,const char *perm,void *d){(void)p;(void)d;return strcmp(perm,"read")==0;}

/* Use one clock domain and preserve a known-good row on setter refusal. */
static void check_session_state(void)
{
    UmiWebSessionStore *store = NULL;
    UmiWebSession sample = {.id = "fixture", .principal = "learner", .created_ns = 20U, .expires_ns = 40U, .active = 1};
    assert(umi_web_session_store_create(&store) == UMI_STATUS_OK);
    assert(umi_web_session_put(store, &sample) == UMI_STATUS_OK);
    assert(umi_web_session_get(store, "fixture", 19U) == NULL);
    assert(umi_web_session_get(store, "fixture", 20U) != NULL);
    assert(umi_web_session_get(store, "fixture", 39U) != NULL);
    assert(umi_web_session_get(store, "fixture", 40U) == NULL);

    UmiWebSession invalid = sample;
    invalid.expires_ns = invalid.created_ns;
    assert(umi_web_session_put(store, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
    invalid = sample;
    memset(invalid.principal, 'x', sizeof(invalid.principal));
    assert(umi_web_session_put(store, &invalid) == UMI_STATUS_CAPACITY_EXCEEDED);
    invalid = sample;
    memset(invalid.id, 'x', sizeof(invalid.id));
    assert(umi_web_session_put(store, &invalid) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_web_session_get(store, invalid.id, 20U) == NULL);
    assert(umi_web_session_revoke(store, invalid.id) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_web_session_count(store) == 1U);
    const UmiWebSession *retained = umi_web_session_get(store, sample.id, 20U);
    assert(retained && strcmp(retained->principal, "learner") == 0 && retained->expires_ns == 40U);
    assert(umi_web_session_prune(store, 39U) == 0U);
    assert(umi_web_session_prune(store, 40U) == 1U);
    umi_web_session_store_destroy(store);
}

static void check_reclaimed_capacity(void)
{
    UmiWebSessionStore *store = NULL;
    UmiWebSession open = {.id = "open", .created_ns = 20U, .active = 1};
    UmiWebSession bounded = {.created_ns = 20U, .expires_ns = 40U, .active = 1};
    assert(umi_web_session_store_create(&store) == UMI_STATUS_OK);
    assert(umi_web_session_put(store, &open) == UMI_STATUS_OK);
    for (size_t i = 1U; i < UMI_WEB_MAX_SESSIONS; ++i) {
        assert(snprintf(bounded.id, sizeof(bounded.id), "fixture-%zu", i) > 0);
        assert(umi_web_session_put(store, &bounded) == UMI_STATUS_OK);
    }
    assert(umi_web_session_count(store) == UMI_WEB_MAX_SESSIONS);
    bounded.expires_ns = 80U;
    assert(umi_web_session_put(store, &bounded) == UMI_STATUS_OK);
    UmiWebSession next = {.id = "next", .created_ns = 20U, .active = 1};
    assert(umi_web_session_put(store, &next) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_web_session_revoke(store, "fixture-1") == UMI_STATUS_OK);
    assert(umi_web_session_prune(store, 40U) == UMI_WEB_MAX_SESSIONS - 2U);
    assert(umi_web_session_count(store) == 2U);
    assert(umi_web_session_get(store, open.id, UINT64_MAX) != NULL);
    assert(umi_web_session_get(store, bounded.id, 40U) != NULL);
    assert(umi_web_session_put(store, &next) == UMI_STATUS_OK);
    assert(umi_web_session_count(store) == 3U);
    umi_web_session_store_destroy(store);
}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWebSessionStore *s = NULL;
    UmiWebOriginPolicy *o = NULL;
    UmiWebSecurity sec;
    UmiWebSession session = {{0}, {0}, 0U, 100U, 1};
    (void)umi_web_copy_text(session.id, sizeof(session.id), "s1");
    (void)umi_web_copy_text(session.principal, sizeof(session.principal), "sam");
    assert(umi_web_session_store_create(&s) == UMI_STATUS_OK);
    assert(umi_web_session_put(s, &session) == UMI_STATUS_OK);
    assert(umi_web_session_get(s, "s1", 1U) != NULL);
    assert(umi_web_origin_policy_create(&o) == UMI_STATUS_OK);
    assert(umi_web_origin_allow(o, "https://local") == UMI_STATUS_OK);
    assert(umi_web_origin_is_allowed(o, "https://local"));
    assert(umi_web_security_init(&sec, allow, NULL) == UMI_STATUS_OK);
    assert(umi_web_security_require(&sec, "sam", "read") == UMI_STATUS_OK);
    assert(umi_web_security_require(&sec, "sam", "write") == UMI_STATUS_PERMISSION_DENIED);
    umi_web_origin_policy_destroy(o);
    umi_web_session_store_destroy(s);
    check_session_state();
    check_reclaimed_capacity();
    return 0;
}
