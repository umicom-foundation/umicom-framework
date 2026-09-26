/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/links.c
 *
 * PURPOSE:
 *   Keep deep links an inert, strictly parsed description until a user reviews them.
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

UmiStatus UmiDeskFederationLinkParse(const char *text, UmiDeskFederationLink *outLink)
{
    UmiDeskFederationLink link = {0};
    const char *id;
    size_t idCapacity;
    if (text == NULL || outLink == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strncmp(text, "umicom://app/", 13U) == 0) { link.kind = UMI_DESK_LINK_APPLICATION; id = text + 13U; }
    else if (strncmp(text, "umicom://group/", 15U) == 0) { link.kind = UMI_DESK_LINK_GROUP; id = text + 15U; }
    else if (strncmp(text, "umicom://session/", 17U) == 0) { link.kind = UMI_DESK_LINK_SESSION; id = text + 17U; }
    else return UMI_STATUS_INVALID_ARGUMENT;
    idCapacity = link.kind == UMI_DESK_LINK_APPLICATION ? sizeof link.id : UMI_DESK_FEDERATION_WORKSPACE_ID_CAPACITY;
    if (DfCopy(link.id, idCapacity, id) != UMI_STATUS_OK || !DfIdValid(link.id, sizeof link.id))
        return UMI_STATUS_INVALID_ARGUMENT;
    *outLink = link;
    return UMI_STATUS_OK;
}
UmiStatus UmiDeskFederationLinkFormat(const UmiDeskFederationLink *link, char *output, size_t capacity)
{
    const char *prefix;
    size_t prefixLength, idLength;
    char text[UMI_DESK_FEDERATION_LINK_CAPACITY];
    if (link == NULL || output == NULL || !DfIdValid(link->id, sizeof link->id)) return UMI_STATUS_INVALID_ARGUMENT;
    switch (link->kind) {
    case UMI_DESK_LINK_APPLICATION: prefix = "umicom://app/"; break;
    case UMI_DESK_LINK_GROUP: prefix = "umicom://group/"; break;
    case UMI_DESK_LINK_SESSION: prefix = "umicom://session/"; break;
    default: return UMI_STATUS_INVALID_ARGUMENT;
    }
    idLength = strlen(link->id); prefixLength = strlen(prefix);
    if (link->kind != UMI_DESK_LINK_APPLICATION && idLength >= UMI_DESK_FEDERATION_WORKSPACE_ID_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < prefixLength + idLength + 1U) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(text, prefix, prefixLength); memcpy(text + prefixLength, link->id, idLength + 1U);
    memcpy(output, text, prefixLength + idLength + 1U);
    return UMI_STATUS_OK;
}
