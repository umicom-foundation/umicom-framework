/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/internal.h
 *
 * PURPOSE:
 *   Keep owned state, CSV parsing and storage private to the enterprise service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_ENTERPRISE_WORKSPACE_INTERNAL_H
#define UMICOM_ENTERPRISE_WORKSPACE_INTERNAL_H
#include "umicom/enterprise_workspace/workspace.h"
#define EWS_WIRE_CAPACITY (2U * 1024U * 1024U)
#define EWS_CHUNK_SIZE 3000U
#define EWS_HEAD_CAPACITY 160U
#define EWS_PREFIX "enterprise.workspace/"
#define EWS_HEAD EWS_PREFIX "head"
typedef struct EwsDataset {
    UmiEnterpriseDataset info;
    UmiEnterpriseRow rows[UMI_ENTERPRISE_MAX_ROWS];
} EwsDataset;
typedef struct EwsJob {
    UmiEnterpriseJob info;
    UmiEnterprisePreview preview;
    char csv[UMI_ENTERPRISE_CSV_CAPACITY];
} EwsJob;
typedef struct EwsState {
    uint64_t revision;
    bool paused;
    bool recipes[UMI_ENTERPRISE_RECIPE_COUNT];
    size_t datasetCount, jobCount, auditCount;
    EwsDataset datasets[UMI_ENTERPRISE_MAX_DATASETS];
    EwsJob jobs[UMI_ENTERPRISE_MAX_JOBS];
    UmiEnterpriseAuditEntry audit[UMI_ENTERPRISE_MAX_AUDIT];
} EwsState;
struct UmiEnterpriseWorkspace {
    UmiDataServer *data;
    UmiAuthorisationService *authorisation;
    EwsState *state;
    char head[EWS_HEAD_CAPACITY];
    bool storageFault;
};
bool EwsId(const char *text);
bool EwsUtf8(const char *text, size_t length, bool multiline);
bool EwsText(const char *text, size_t capacity, bool empty, bool multiline);
UmiStatus EwsCopy(char *destination, size_t capacity, const char *source);
size_t EwsDatasetIndex(const EwsState *state, const char *id);
size_t EwsJobIndex(const EwsState *state, const char *id);
size_t EwsRecipeIndex(const char *id);
void EwsRecipe(size_t index, UmiEnterpriseRecipe *outRecipe);
UmiStatus EwsParse(const char *csv, size_t length, size_t recipe,
    UmiEnterpriseRow *rows, size_t *outCount, UmiEnterpriseIssue *issue);
UmiStatus EwsPreview(const EwsState *state, const char *datasetId,
    const char *recipeId, const char *csv, size_t length,
    UmiEnterprisePreview *outPreview, UmiEnterpriseIssue *issue);
UmiStatus EwsValidate(const EwsState *state);
UmiStatus EwsEncode(const EwsState *state, char **outText, size_t *outLength);
UmiStatus EwsDecode(const char *text, size_t length, EwsState *state);
UmiStatus EwsLoad(UmiEnterpriseWorkspace *workspace, EwsState *state,
    char head[EWS_HEAD_CAPACITY]);
UmiStatus EwsCurrent(UmiEnterpriseWorkspace *workspace);
UmiStatus EwsSave(UmiEnterpriseWorkspace *workspace, const EwsState *state);
UmiStatus EwsCommit(UmiEnterpriseWorkspace *workspace, EwsState *next,
    UmiEnterpriseActor actor, const char *action, const char *target, const char *detail);
#endif
