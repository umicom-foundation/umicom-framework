/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_uri.c
 * PURPOSE: Verify reversible percent-safe absolute file URI conversion.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include "umicom/document/document.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char current[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char decoded[UMI_PATH_CAPACITY];
    char uri[UMI_DOCUMENT_URI_CAPACITY];
    assert(umi_fs_current_directory(current, sizeof(current)) == UMI_STATUS_OK);
    assert(umi_path_join(current, "A file #1.c", path, sizeof(path)) == UMI_STATUS_OK);
    assert(umi_document_uri_from_path(path, uri, sizeof(uri)) == UMI_STATUS_OK);
    assert(strncmp(uri, "file://", 7U) == 0);
    assert(strstr(uri, "%20") != NULL && strstr(uri, "%23") != NULL);
    assert(umi_document_uri_to_path(uri, decoded, sizeof(decoded)) == UMI_STATUS_OK);
    assert(umi_path_equal(path, decoded));
    /* URI identity is independent of GTK/Windows locale selection and safely
     * represents unsaved names without granting them filesystem semantics. */
    assert(umi_document_uri_from_untitled_name("Private filename.c", uri, sizeof(uri)) == UMI_STATUS_OK);
    assert(strcmp(uri, "untitled:///Private%20filename.c") == 0);
    assert(umi_document_uri_from_untitled_name("caf\xc3\xa9 #100%/a:b.c", uri, sizeof(uri)) == UMI_STATUS_OK);
    assert(strcmp(uri, "untitled:///caf%C3%A9%20%23100%25%2Fa%3Ab.c") == 0);
    assert(umi_document_uri_from_untitled_name("Example.c", uri, sizeof(uri)) == UMI_STATUS_OK);
    assert(strcmp(uri, "untitled:///Example.c") == 0);
    strcpy(uri, "unchanged");
    assert(umi_document_uri_from_untitled_name(" ", uri, 15U) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(strcmp(uri, "unchanged") == 0);
    assert(umi_document_uri_from_untitled_name(" ", uri, 16U) == UMI_STATUS_OK);
    assert(strcmp(uri, "untitled:///%20") == 0);
    strcpy(uri, "unchanged");
    assert(umi_document_uri_from_untitled_name("\xc3", uri, sizeof(uri)) == UMI_STATUS_INVALID_ARGUMENT);
    assert(strcmp(uri, "unchanged") == 0);
    assert(umi_document_uri_from_untitled_name("", uri, sizeof(uri)) == UMI_STATUS_INVALID_ARGUMENT);
    strcpy(uri, "A name.c");
    assert(umi_document_uri_from_untitled_name(uri, uri, sizeof(uri)) == UMI_STATUS_OK);
    assert(strcmp(uri, "untitled:///A%20name.c") == 0);
    assert(umi_path_join(current, "caf\xc3\xa9.txt", path, sizeof(path)) == UMI_STATUS_OK);
    assert(umi_document_uri_from_path(path, uri, sizeof(uri)) == UMI_STATUS_OK);
    assert(strstr(uri, "caf%C3%A9.txt") != NULL);
    assert(umi_document_uri_to_path(uri, decoded, sizeof(decoded)) == UMI_STATUS_OK);
    assert(umi_path_equal(path, decoded));
    /* Malformed or undersized conversions must not replace an editor's last
     * valid filename with a partial path or a prefix ending at encoded NUL. */
    char expected_uri[UMI_DOCUMENT_URI_CAPACITY];
    char malformed[UMI_DOCUMENT_URI_CAPACITY];
    assert(umi_document_uri_from_path(path, expected_uri, sizeof expected_uri) == UMI_STATUS_OK);
    strcpy(decoded, "unchanged");
    assert(umi_document_uri_to_path(expected_uri, decoded, 3U) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(strcmp(decoded, "unchanged") == 0);
    strcpy(uri, "unchanged");
    assert(umi_document_uri_from_path(path, uri, 8U) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(strcmp(uri, "unchanged") == 0);
    assert(umi_document_uri_from_path(path, uri, strlen(expected_uri) + 1U) == UMI_STATUS_OK);
    assert(strcmp(uri, expected_uri) == 0);
    const char *tails[] = {"%00hidden", "%C0%AF", "%", "%0", "%GG", "#fragment", "?query", " raw"};
    for (size_t index = 0U; index < sizeof tails / sizeof tails[0]; ++index) {
        int written = snprintf(malformed, sizeof malformed, "%s%s", expected_uri, tails[index]);
        assert(written > 0 && (size_t)written < sizeof malformed);
        strcpy(decoded, "unchanged");
        assert(umi_document_uri_to_path(malformed, decoded, sizeof decoded) == UMI_STATUS_INVALID_ARGUMENT);
        assert(strcmp(decoded, "unchanged") == 0);
    }
    strcpy(uri, expected_uri);
    assert(umi_document_uri_to_path(uri, uri, sizeof uri) == UMI_STATUS_OK);
    assert(umi_path_equal(uri, path));
    assert(umi_document_uri_from_path(uri, uri, sizeof uri) == UMI_STATUS_OK);
    assert(strcmp(uri, expected_uri) == 0);
    strcpy(decoded, "unchanged");
    assert(umi_document_uri_to_path("file://server/share", decoded, sizeof decoded) == UMI_STATUS_INVALID_ARGUMENT);
    assert(strcmp(decoded, "unchanged") == 0);
    assert(umi_document_uri_to_path("file://", decoded, sizeof decoded) == UMI_STATUS_INVALID_ARGUMENT);
    assert(strcmp(decoded, "unchanged") == 0);
    assert(umi_path_join(current, "bad-\xC3", malformed, sizeof malformed) == UMI_STATUS_OK);
    strcpy(uri, "unchanged");
    assert(umi_document_uri_from_path(malformed, uri, sizeof uri) == UMI_STATUS_INVALID_ARGUMENT);
    assert(strcmp(uri, "unchanged") == 0);
#ifdef _WIN32
    /* Preserve the established native UNC round trip. This does not interpret
     * a hostname authority as a local relative filename. */
    assert(umi_document_uri_from_path("\\\\server\\share\\notes.txt", uri, sizeof uri) == UMI_STATUS_OK);
    assert(umi_document_uri_to_path(uri, decoded, sizeof decoded) == UMI_STATUS_OK);
    assert(umi_path_equal(decoded, "\\\\server\\share\\notes.txt"));
#endif
    (void)printf("document URI passed\n");
    return 0;
}
