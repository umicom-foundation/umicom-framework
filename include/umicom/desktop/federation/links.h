/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/federation/links.h
 *
 * PURPOSE:
 *   Parse in-application Umicom links without dispatching commands or opening URLs.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_FEDERATION_LINKS_H
#define UMICOM_DESKTOP_FEDERATION_LINKS_H
#include "umicom/desktop/federation/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiDeskFederationLinkKind {
    UMI_DESK_LINK_APPLICATION = 1,
    UMI_DESK_LINK_GROUP = 2,
    UMI_DESK_LINK_SESSION = 3
} UmiDeskFederationLinkKind;
typedef struct UmiDeskFederationLink {
    UmiDeskFederationLinkKind kind;
    char id[UMI_DESK_FEDERATION_APP_ID_CAPACITY];
} UmiDeskFederationLink;
/* Supported grammar: umicom://app/<id>, umicom://group/<id>,
 * umicom://session/<id>. ASCII IDs start with an alphanumeric character and
 * contain only letters, numbers, dots, underscores and hyphens. Queries,
 * fragments, encoded characters, path separators and additional slashes are
 * rejected. Links are reviewed INSIDE Desk, not registered with the OS. */
UmiStatus UmiDeskFederationLinkParse(const char *text, UmiDeskFederationLink *outLink);
UmiStatus UmiDeskFederationLinkFormat(const UmiDeskFederationLink *link,
    char *output, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
