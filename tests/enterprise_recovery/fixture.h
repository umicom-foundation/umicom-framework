/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_recovery/fixture.h
 * PURPOSE: Share owned test setup and assertions across independent enterprise regressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_ENTERPRISE_RECOVERY_FIXTURE_H
#define UMICOM_ENTERPRISE_RECOVERY_FIXTURE_H
#include "umicom/enterprise_workspace/practice.h"
#include "umicom/enterprise_workspace/recovery.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)
#define OK(x) REQUIRE((x)==UMI_STATUS_OK)
#define STATUS(x, s) REQUIRE((x)==(s))
#define ACT(i) UmiEnterprisePracticeActor(i)
typedef struct TestFixture {
    UmiDataServer *data;
    UmiEnterprisePracticeAccess *access;
    UmiEnterpriseWorkspace *workspace;
} TestFixture;
int TestOpen(TestFixture *f, const char *path);
void TestClose(TestFixture *f);
int TestSeed(TestFixture *f);
int TestApply(TestFixture *f, const char *id, const char *csv);
int TestStale(TestFixture *f);
uint64_t TestRevision(TestFixture *f);
int TestQueries(const char *name);
int TestRecovery(const char *name, const char *path);
int TestStorage(const char *name, const char *path);
#endif
