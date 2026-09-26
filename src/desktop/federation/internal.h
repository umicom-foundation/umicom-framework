/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/internal.h
 *
 * PURPOSE:
 *   Keep storage and review ownership private to the Framework capability.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_FEDERATION_INTERNAL_H
#define UMICOM_DESKTOP_FEDERATION_INTERNAL_H
#include "umicom/desktop/federation/service.h"

typedef struct DfStore {
    UmiDeskWorkspace workspaces[UMI_DESK_FEDERATION_MAX_WORKSPACES];
    size_t count;
    uint64_t revision;
} DfStore;
struct UmiDeskFederation {
    UmiDataServer *server;
    UmiDeskFederationPorts ports;
    char hostId[UMI_DESK_FEDERATION_APP_ID_CAPACITY];
    DfStore *store;
    bool loaded, busy, poisoned, observed;
    uint64_t nextToken;
    UmiDeskFederationPlan plan;
    UmiDeskFederationCatalogue baseline;
    UmiDeskFederationActivity activity[UMI_DESK_FEDERATION_MAX_ACTIVITY];
    size_t activityStart, activityCount;
    uint64_t activitySequence, droppedActivity;
};
bool DfIdValid(const char *text, size_t capacity);
bool DfTextValid(const char *text, size_t capacity);
UmiStatus DfCopy(char *destination, size_t capacity, const char *source);
UmiStatus DfWorkspaceValidate(const UmiDeskWorkspace *workspace);
UmiStatus DfStoreValidate(const DfStore *store);
UmiStatus DfCatalogueRead(UmiDeskFederation *service, UmiDeskFederationCatalogue *outCatalogue);
const UmiDeskFederationApp *DfAppFind(const UmiDeskFederationCatalogue *catalogue, const char *id);
bool DfAppSame(const UmiDeskFederationApp *left, const UmiDeskFederationApp *right);
size_t DfWorkspaceIndex(const DfStore *store, UmiDeskWorkspaceKind kind, const char *id);
UmiStatus DfEnter(UmiDeskFederation *service, bool requireLoaded);
UmiStatus DfRepositoryLoad(UmiDeskFederation *service, DfStore *outStore);
UmiStatus DfRepositoryCommit(UmiDeskFederation *service, const DfStore *candidate);
UmiStatus DfRepositoryCheck(UmiDeskFederation *service);
UmiStatus DfSave(UmiDeskFederation *service, const UmiDeskWorkspace *workspace);
UmiStatus DfPrepare(UmiDeskFederation *service, const UmiDeskFederationAppId *members,
    size_t count, UmiDeskFederationPlan *outPlan);
UmiDeskFederationPlanItem DfClassify(UmiDeskFederation *service,
    const UmiDeskFederationCatalogue *catalogue, const UmiDeskFederationAppId *member);
void DfActivityPush(UmiDeskFederation *service, const UmiDeskFederationApp *app,
    UmiDeskFederationActivityKind kind, UmiStatus status);
/* Storage codec has a strict, versioned record format. Numbers use canonical
 * unsigned decimal; titles use hex-encoded validated UTF-8, never raw structs. */
UmiStatus DfParseUnsigned(const char *text, uint64_t *outValue);
UmiStatus DfEncodeHeader(const UmiDeskWorkspace *workspace, char *output, size_t capacity);
UmiStatus DfDecodeHeader(const char *text, UmiDeskWorkspace *outWorkspace);
#endif
