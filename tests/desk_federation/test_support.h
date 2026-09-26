/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_support.h
 *
 * PURPOSE:
 *   Supply inert host fixtures while using the real Framework service and Data Server.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESK_FEDERATION_TEST_SUPPORT_H
#define UMICOM_DESK_FEDERATION_TEST_SUPPORT_H
#include "umicom/desktop/federation/service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#define CHECK(expression) do { if (!(expression)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; } } while (0)
#define OK(expression) CHECK((expression) == UMI_STATUS_OK)
typedef struct TestHost {
    UmiDeskFederationCatalogue catalogue;
    size_t readCount, requestCount;
    UmiStatus readStatus;
    UmiStatus requestStatus[UMI_DESK_FEDERATION_MAX_APPS];
    UmiDeskFederationAction actions[UMI_DESK_FEDERATION_MAX_APPS];
    UmiDeskFederationAppId requests[UMI_DESK_FEDERATION_MAX_APPS];
    UmiDeskFederation *service;
    bool mutateSecond, reenter;
    UmiStatus reentryStatus;
} TestHost;
typedef struct TestFixture {
    UmiDataServer *server;
    UmiDeskFederation *service;
    TestHost host;
} TestFixture;
UmiStatus TestRead(void *context, UmiDeskFederationCatalogue *outCatalogue);
UmiStatus TestRequest(void *context, const UmiDeskFederationApp *expected, UmiDeskFederationAction action);
void TestCatalogue(TestHost *host);
UmiStatus TestOpen(TestFixture *fixture, const char *path);
void TestClose(TestFixture *fixture);
UmiDeskWorkspace TestWorkspace(const char *id, size_t members);
uint64_t TestRevision(UmiDeskFederation *service);
int TestGroups(const char *name, const char *path);
int TestReview(const char *name, const char *path);
int TestLinks(const char *name, const char *path);
int TestStorage(const char *name, const char *path);
int TestActivity(const char *name, const char *path);
#endif
