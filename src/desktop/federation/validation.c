/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/validation.c
 *
 * PURPOSE:
 *   Reject malformed identities and snapshots before saving or executing them.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <string.h>
#include <stdlib.h>

bool DfIdValid(const char *text, size_t capacity)
{
    size_t length;
    if (text == NULL || capacity < 2U || memchr(text, '\0', capacity) == NULL) return false;
    length = strlen(text);
    if (length == 0U) return false;
    for (size_t index = 0U; index < length; ++index) {
        const unsigned char ch = (unsigned char)text[index];
        const bool alphaNumeric = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9');
        if (!alphaNumeric && (index == 0U || (ch != '.' && ch != '_' && ch != '-'))) return false;
    }
    return true;
}

/* Validate UTF-8 without adopting a GUI or locale dependency. Reject overlong
 * sequences, surrogates, out-of-range scalars, C0/C1 control characters and
 * line/paragraph separators. Names may contain ordinary non-Latin text. */
bool DfTextValid(const char *text, size_t capacity)
{
    size_t length, index = 0U;
    if (text == NULL || memchr(text, '\0', capacity) == NULL) return false;
    length = strlen(text);
    if (length == 0U) return false;
    while (index < length) {
        uint32_t scalar, minimum;
        size_t remaining;
        const unsigned char first = (unsigned char)text[index++];
        if (first < 0x80U) { scalar = first; minimum = 0U; remaining = 0U; }
        else if (first >= 0xC2U && first <= 0xDFU) { scalar = first & 0x1FU; minimum = 0x80U; remaining = 1U; }
        else if (first >= 0xE0U && first <= 0xEFU) { scalar = first & 0x0FU; minimum = 0x800U; remaining = 2U; }
        else if (first >= 0xF0U && first <= 0xF4U) { scalar = first & 0x07U; minimum = 0x10000U; remaining = 3U; }
        else return false;
        if (remaining > length - index) return false;
        while (remaining-- > 0U) {
            const unsigned char next = (unsigned char)text[index++];
            if ((next & 0xC0U) != 0x80U) return false;
            scalar = (scalar << 6U) | (uint32_t)(next & 0x3FU);
        }
        if (scalar < minimum || scalar > 0x10FFFFU || (scalar >= 0xD800U && scalar <= 0xDFFFU) ||
            scalar < 0x20U || (scalar >= 0x7FU && scalar <= 0x9FU) || scalar == 0x2028U || scalar == 0x2029U)
            return false;
    }
    return true;
}

UmiStatus DfCopy(char *destination, size_t capacity, const char *source)
{
    size_t length = 0U;
    if (destination == NULL || source == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Unlike memchr on a caller's short string, this loop stops at its first
     * terminator and never assumes the input allocation is capacity bytes. */
    while (length < capacity && source[length] != '\0') ++length;
    if (length == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memmove(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
UmiStatus DfWorkspaceValidate(const UmiDeskWorkspace *workspace)
{
    if (workspace == NULL ||
        (workspace->kind != UMI_DESK_WORKSPACE_GROUP && workspace->kind != UMI_DESK_WORKSPACE_SESSION) ||
        !DfIdValid(workspace->id, sizeof workspace->id) ||
        !DfTextValid(workspace->title, sizeof workspace->title) ||
        workspace->memberCount == 0U || workspace->memberCount > UMI_DESK_FEDERATION_MAX_APPS)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < workspace->memberCount; ++index) {
        if (!DfIdValid(workspace->members[index].value, sizeof workspace->members[index].value))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t prior = 0U; prior < index; ++prior)
            if (strcmp(workspace->members[index].value, workspace->members[prior].value) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}
UmiStatus DfStoreValidate(const DfStore *store)
{
    if (store == NULL || store->count > UMI_DESK_FEDERATION_MAX_WORKSPACES) return UMI_STATUS_PARSE_ERROR;
    for (size_t index = 0U; index < store->count; ++index) {
        if (DfWorkspaceValidate(&store->workspaces[index]) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
        for (size_t prior = 0U; prior < index; ++prior)
            if (store->workspaces[index].kind == store->workspaces[prior].kind &&
                strcmp(store->workspaces[index].id, store->workspaces[prior].id) == 0)
                return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
UmiStatus DfCatalogueRead(UmiDeskFederation *service, UmiDeskFederationCatalogue *outCatalogue)
{
    UmiStatus status;
    memset(outCatalogue, 0, sizeof *outCatalogue);
    status = service->ports.read(service->ports.context, outCatalogue);
    if (status != UMI_STATUS_OK) return status;
    if (outCatalogue->count > UMI_DESK_FEDERATION_MAX_APPS) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < outCatalogue->count; ++index) {
        const UmiDeskFederationApp *app = &outCatalogue->apps[index];
        if (!DfIdValid(app->id.value, sizeof app->id.value) || !DfTextValid(app->title, sizeof app->title) ||
            app->state < UMI_DESK_FEDERATION_UNKNOWN || app->state > UMI_DESK_FEDERATION_STOPPING)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t prior = 0U; prior < index; ++prior)
            if (strcmp(app->id.value, outCatalogue->apps[prior].id.value) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}
const UmiDeskFederationApp *DfAppFind(const UmiDeskFederationCatalogue *catalogue, const char *id)
{
    for (size_t index = 0U; index < catalogue->count; ++index)
        if (strcmp(catalogue->apps[index].id.value, id) == 0) return &catalogue->apps[index];
    return NULL;
}
bool DfAppSame(const UmiDeskFederationApp *left, const UmiDeskFederationApp *right)
{
    return strcmp(left->id.value, right->id.value) == 0 && left->installed == right->installed &&
        left->compatible == right->compatible && left->enabled == right->enabled &&
        left->running == right->running && left->state == right->state &&
        left->processToken == right->processToken && left->generation == right->generation;
}
size_t DfWorkspaceIndex(const DfStore *store, UmiDeskWorkspaceKind kind, const char *id)
{
    for (size_t index = 0U; index < store->count; ++index)
        if (store->workspaces[index].kind == kind && strcmp(store->workspaces[index].id, id) == 0) return index;
    return store->count;
}
UmiStatus DfEnter(UmiDeskFederation *service, bool requireLoaded)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (service->busy) return UMI_STATUS_BUSY;
    if (service->poisoned) return UMI_STATUS_IO_ERROR;
    if (requireLoaded && !service->loaded) return UMI_STATUS_INVALID_STATE;
    service->busy = true;
    return UMI_STATUS_OK;
}
