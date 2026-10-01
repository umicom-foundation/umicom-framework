/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_ui/source_navigation.c
 * PURPOSE: Keep test navigation and document lifetime rules reusable across hosts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_ui/source_navigation.h"
#include "umicom/diagnostic_ui/navigation.h"
#include <string.h>
static UmiStatus OpenAt(UmiDocumentCoordinator *documents, const char *path,
    size_t line, size_t column, const char *message, UmiDiagnosticSeverity severity,
    const char *baseDirectory, size_t *outOffset)
{
    UmiDiagnosticSnapshot diagnostic;
    if (documents == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (path[0] == '\0' || line == 0U) return UMI_STATUS_NOT_FOUND;
    if (strlen(path) >= sizeof(diagnostic.uri) || line > UINT32_MAX || column > UINT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_diagnostic_snapshot_init(&diagnostic, "test.source", severity,
        UMI_DIAGNOSTIC_KIND_TEST, "test", message[0] != '\0' ? message : "Test source location");
    if (status != UMI_STATUS_OK) return status;
    strcpy(diagnostic.uri, path);
    diagnostic.line = (uint32_t)line; diagnostic.column = (uint32_t)column;
    return UmiDiagnosticOpenSource(documents, &diagnostic, baseDirectory, outOffset);
}
UmiStatus UmiTestSourceLinkOpen(UmiDocumentCoordinator *documents,
    const UmiTestSourceLink *link, const char *baseDirectory, size_t *outOffset)
{
    if (link == NULL || memchr(link->item_id, '\0', sizeof(link->item_id)) == NULL ||
        link->item_id[0] == '\0' || memchr(link->session_id, '\0', sizeof(link->session_id)) == NULL ||
        memchr(link->record_id, '\0', sizeof(link->record_id)) == NULL || link->record_id[0] == '\0' ||
        link->origin < UMI_TEST_SOURCE_RESULT_DETAILS || link->origin > UMI_TEST_SOURCE_OUTPUT ||
        memchr(link->location.path, '\0', sizeof(link->location.path)) == NULL ||
        memchr(link->location.message, '\0', sizeof(link->location.message)) == NULL ||
        memchr(link->location.code, '\0', sizeof(link->location.code)) == NULL ||
        link->location.severity < UMI_DIAGNOSTIC_TRACE || link->location.severity > UMI_DIAGNOSTIC_FATAL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return OpenAt(documents, link->location.path, link->location.line, link->location.column,
        link->location.message, link->location.severity, baseDirectory, outOffset);
}
UmiStatus UmiTestItemOpenSource(UmiDocumentCoordinator *documents,
    const UmiTestPlatformItemSnapshot *item, const char *baseDirectory, size_t *outOffset)
{
    if (item == NULL || memchr(item->source_uri, '\0', sizeof(item->source_uri)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return OpenAt(documents, item->source_uri, item->source_line != 0U ? item->source_line : 1U,
        0U, "Discovered test source", UMI_DIAGNOSTIC_INFO, baseDirectory, outOffset);
}
