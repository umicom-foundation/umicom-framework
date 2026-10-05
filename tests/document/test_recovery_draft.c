/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_recovery_draft.c
 * PURPOSE: Exercise exact recovery framing, independent source ownership and damaged-record rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/recovery_draft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"round-trip",
                                            "unicode",
                                            "crlf",
                                            "bare-cr",
                                            "empty",
                                            "large",
                                            "maximum",
                                            "source-owned",
                                            "encoded-owned",
                                            "name-empty",
                                            "name-control",
                                            "name-unterminated",
                                            "path-control",
                                            "language-invalid",
                                            "key-short",
                                            "key-upper",
                                            "key-long",
                                            "key-invalid",
                                            "text-nul",
                                            "text-invalid",
                                            "cursor-out",
                                            "selection-out",
                                            "cursor-scalar",
                                            "selection-scalar",
                                            "cursor-crlf",
                                            "selection-crlf",
                                            "size-mismatch",
                                            "source-limit",
                                            "decode-limit",
                                            "cancelled-create",
                                            "cancelled-decode",
                                            "null-create",
                                            "null-decode",
                                            "null-encode",
                                            "null-read",
                                            "truncated",
                                            "trailing",
                                            "magic",
                                            "header-nul",
                                            "header-long",
                                            "field-order",
                                            "negative",
                                            "plus",
                                            "leading-zero",
                                            "overflow",
                                            "empty-number",
                                            "metadata-nul",
                                            "length-mismatch",
                                            "key-generate",
                                            "key-capacity",
                                            "key-null"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    UmiDocumentRecoveryInfo info = {0};
    strcpy(info.key, "0123456789abcdef0123456789abcdef");
    strcpy(info.display_name, "source.c");
    strcpy(info.source_path, "C:/project/source.c");
    strcpy(info.language_id, "c");
    info.source_revision = UINT64_MAX;
    const char *text = "one\ntwo";
    size_t size = strlen(text);
    char *owned = NULL;
    if (strcmp(mode, "unicode") == 0)
    {
        text = "a\xe9\x9b\xaa\xf0\x9f\x98\x80";
        strcpy(info.display_name, "caf\xc3\xa9.c");
        size = strlen(text);
        info.cursor_offset = 1U;
        info.selection_bytes = 7U;
    }
    if (strcmp(mode, "crlf") == 0)
    {
        text = "one\r\ntwo";
        size = strlen(text);
        info.cursor_offset = 5U;
        info.selection_bytes = 3U;
    }
    if (strcmp(mode, "bare-cr") == 0)
    {
        text = "one\rtwo";
        size = strlen(text);
    }
    if (strcmp(mode, "empty") == 0)
    {
        text = "";
        size = 0U;
    }
    if (strcmp(mode, "large") == 0 || strcmp(mode, "maximum") == 0)
    {
        size = strcmp(mode, "maximum") == 0 ? UMI_DOCUMENT_RECOVERY_TEXT_LIMIT : 32768U;
        owned = malloc(size + 1U);
        CHECK(owned);
        memset(owned, 'x', size);
        owned[size] = '\0';
        text = owned;
        info.cursor_offset = size;
    }
    info.text_bytes = size;
    UmiDocumentRecoveryDraft *draft = NULL, *decoded = NULL;
    unsigned char *encoded = NULL;
    size_t encoded_size = 0U;
    CHECK(UmiDocumentRecoveryDraftCreate(&info, text, size, NULL, &draft) == UMI_STATUS_OK);
    CHECK(UmiDocumentRecoveryDraftEncode(draft, &encoded, &encoded_size) == UMI_STATUS_OK);
    CHECK(UmiDocumentRecoveryDraftDecode(encoded, encoded_size, NULL, &decoded) == UMI_STATUS_OK);
    UmiDocumentRecoveryInfo actual;
    const char *body = NULL;
    size_t bytes = 0U;
    CHECK(UmiDocumentRecoveryDraftInspect(decoded, &actual) == UMI_STATUS_OK);
    CHECK(strcmp(actual.key, info.key) == 0 && strcmp(actual.display_name, info.display_name) == 0 &&
          strcmp(actual.source_path, info.source_path) == 0 &&
          strcmp(actual.language_id, info.language_id) == 0);
    CHECK(actual.source_revision == info.source_revision && actual.text_bytes == size &&
          actual.cursor_offset == info.cursor_offset && actual.selection_bytes == info.selection_bytes);
    CHECK(UmiDocumentRecoveryDraftRead(decoded, &body, &bytes) == UMI_STATUS_OK && bytes == size &&
          memcmp(body, text, size) == 0 && body[size] == '\0');
    if (strcmp(mode, "source-owned") == 0)
    {
        char input[] = "owned";
        info.text_bytes = 5U;
        UmiDocumentRecoveryDraft *copy = NULL;
        CHECK(UmiDocumentRecoveryDraftCreate(&info, input, 5U, NULL, &copy) == UMI_STATUS_OK);
        memset(input, 'z', 5U);
        CHECK(UmiDocumentRecoveryDraftRead(copy, &body, &bytes) == UMI_STATUS_OK &&
              strcmp(body, "owned") == 0);
        UmiDocumentRecoveryDraftDestroy(copy);
    }
    if (strcmp(mode, "encoded-owned") == 0)
    {
        memset(encoded, 'z', encoded_size);
        CHECK(UmiDocumentRecoveryDraftRead(decoded, &body, &bytes) == UMI_STATUS_OK &&
              memcmp(body, text, size) == 0);
    }
    UmiDocumentRecoveryDraftDestroy(decoded);
    decoded = NULL;
    int invalid = 0;
    if (strcmp(mode, "name-empty") == 0)
    {
        info.display_name[0] = '\0';
        invalid = 1;
    }
    if (strcmp(mode, "name-control") == 0)
    {
        strcpy(info.display_name, "bad\nname");
        invalid = 1;
    }
    if (strcmp(mode, "name-unterminated") == 0)
    {
        memset(info.display_name, 'x', sizeof(info.display_name));
        invalid = 1;
    }
    if (strcmp(mode, "path-control") == 0)
    {
        strcpy(info.source_path, "bad\rpath");
        invalid = 1;
    }
    if (strcmp(mode, "language-invalid") == 0)
    {
        strcpy(info.language_id, "\xff");
        invalid = 1;
    }
    if (strcmp(mode, "key-short") == 0)
    {
        info.key[31] = '\0';
        invalid = 1;
    }
    if (strcmp(mode, "key-upper") == 0)
    {
        info.key[0] = 'A';
        invalid = 1;
    }
    if (strcmp(mode, "key-long") == 0)
    {
        info.key[32] = 'f';
        invalid = 1;
    }
    if (strcmp(mode, "key-invalid") == 0)
    {
        info.key[0] = '/';
        invalid = 1;
    }
    if (strcmp(mode, "text-nul") == 0)
    {
        text = "one\0two";
        size = 7U;
        invalid = 1;
    }
    if (strcmp(mode, "text-invalid") == 0)
    {
        text = "\xc0\x80";
        size = 2U;
        info.text_bytes = size;
        invalid = 1;
    }
    if (strcmp(mode, "cursor-out") == 0)
    {
        info.cursor_offset = size + 1U;
        invalid = 1;
    }
    if (strcmp(mode, "selection-out") == 0)
    {
        info.cursor_offset = 1U;
        info.selection_bytes = SIZE_MAX;
        invalid = 1;
    }
    if (strcmp(mode, "cursor-scalar") == 0 || strcmp(mode, "selection-scalar") == 0)
    {
        text = "a\xe9\x9b\xaa";
        size = 4U;
        info.text_bytes = size;
        info.cursor_offset = strcmp(mode, "cursor-scalar") == 0 ? 2U : 1U;
        info.selection_bytes = strcmp(mode, "selection-scalar") == 0 ? 1U : 0U;
        invalid = 1;
    }
    if (strcmp(mode, "cursor-crlf") == 0 || strcmp(mode, "selection-crlf") == 0)
    {
        text = "a\r\nb";
        size = 4U;
        info.text_bytes = size;
        info.cursor_offset = strcmp(mode, "cursor-crlf") == 0 ? 2U : 1U;
        info.selection_bytes = strcmp(mode, "selection-crlf") == 0 ? 1U : 0U;
        invalid = 1;
    }
    if (strcmp(mode, "size-mismatch") == 0)
    {
        ++info.text_bytes;
        invalid = 1;
    }
    if (invalid)
    {
        CHECK(UmiDocumentRecoveryDraftCreate(&info, text, size, NULL, &decoded) != UMI_STATUS_OK &&
              decoded == NULL);
    }
    if (strcmp(mode, "source-limit") == 0)
        CHECK(UmiDocumentRecoveryDraftCreate(&info, "", UMI_DOCUMENT_RECOVERY_TEXT_LIMIT + 1U, NULL,
                                             &decoded) == UMI_STATUS_CAPACITY_EXCEEDED &&
              decoded == NULL);
    if (strcmp(mode, "decode-limit") == 0)
        CHECK(UmiDocumentRecoveryDraftDecode("", UMI_DOCUMENT_RECOVERY_RECORD_LIMIT + 1U, NULL, &decoded) ==
                  UMI_STATUS_CAPACITY_EXCEEDED &&
              decoded == NULL);
    if (strcmp(mode, "cancelled-create") == 0 || strcmp(mode, "cancelled-decode") == 0)
    {
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        UmiStatus status = strcmp(mode, "cancelled-create") == 0
                               ? UmiDocumentRecoveryDraftCreate(&info, text, size, cancel, &decoded)
                               : UmiDocumentRecoveryDraftDecode(encoded, encoded_size, cancel, &decoded);
        CHECK(status == UMI_STATUS_CANCELLED && decoded == NULL);
        umi_cancellation_token_destroy(cancel);
    }
    if (strcmp(mode, "null-create") == 0)
    {
        CHECK(UmiDocumentRecoveryDraftCreate(NULL, text, size, NULL, &decoded) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              decoded == NULL);
        CHECK(UmiDocumentRecoveryDraftCreate(&info, NULL, size, NULL, &decoded) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentRecoveryDraftCreate(&info, text, size, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    if (strcmp(mode, "null-decode") == 0)
    {
        CHECK(UmiDocumentRecoveryDraftDecode(NULL, 0U, NULL, &decoded) == UMI_STATUS_INVALID_ARGUMENT &&
              decoded == NULL);
        CHECK(UmiDocumentRecoveryDraftDecode(encoded, encoded_size, NULL, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    if (strcmp(mode, "null-encode") == 0)
    {
        unsigned char *bad = encoded;
        size_t count = 9U;
        CHECK(UmiDocumentRecoveryDraftEncode(NULL, &bad, &count) == UMI_STATUS_INVALID_ARGUMENT &&
              bad == NULL && count == 0U);
    }
    if (strcmp(mode, "null-read") == 0)
    {
        body = text;
        bytes = 9U;
        CHECK(UmiDocumentRecoveryDraftRead(NULL, &body, &bytes) == UMI_STATUS_INVALID_ARGUMENT &&
              body == NULL && bytes == 0U);
    }
    if (strcmp(mode, "truncated") == 0)
        for (size_t i = 0U; i < encoded_size; ++i)
            CHECK(UmiDocumentRecoveryDraftDecode(encoded, i, NULL, &decoded) != UMI_STATUS_OK &&
                  decoded == NULL);
    const char *field = NULL, *replacement = NULL;
    if (strcmp(mode, "field-order") == 0)
    {
        field = "revision=";
        replacement = "cursor=";
    }
    if (strcmp(mode, "negative") == 0)
    {
        field = "cursor=0";
        replacement = "cursor=-1";
    }
    if (strcmp(mode, "plus") == 0)
    {
        field = "cursor=0";
        replacement = "cursor=+1";
    }
    if (strcmp(mode, "leading-zero") == 0)
    {
        field = "cursor=0";
        replacement = "cursor=01";
    }
    if (strcmp(mode, "overflow") == 0)
    {
        field = "cursor=0";
        replacement = "cursor=18446744073709551616";
    }
    if (strcmp(mode, "empty-number") == 0)
    {
        field = "cursor=0";
        replacement = "cursor=";
    }
    if (strcmp(mode, "length-mismatch") == 0)
    {
        field = "text-bytes=7";
        replacement = "text-bytes=8";
    }
    if (field != NULL)
    {
        char *copy = calloc(encoded_size + 256U, 1U);
        CHECK(copy);
        memcpy(copy, encoded, encoded_size);
        char *at = strstr(copy, field);
        CHECK(at);
        size_t offset = (size_t)(at - copy), old = strlen(field), fresh = strlen(replacement);
        memmove(at + fresh, at + old, encoded_size - offset - old);
        memcpy(at, replacement, fresh);
        CHECK(UmiDocumentRecoveryDraftDecode(copy, encoded_size - old + fresh, NULL, &decoded) !=
                  UMI_STATUS_OK &&
              decoded == NULL);
        free(copy);
    }
    if (strcmp(mode, "trailing") == 0)
    {
        unsigned char *copy = malloc(encoded_size + 1U);
        CHECK(copy);
        memcpy(copy, encoded, encoded_size);
        copy[encoded_size] = 'x';
        CHECK(UmiDocumentRecoveryDraftDecode(copy, encoded_size + 1U, NULL, &decoded) != UMI_STATUS_OK &&
              decoded == NULL);
        free(copy);
    }
    if (strcmp(mode, "magic") == 0 || strcmp(mode, "header-nul") == 0 || strcmp(mode, "metadata-nul") == 0)
    {
        size_t at = 0U;
        if (strcmp(mode, "metadata-nul") == 0)
        {
            char *copy = calloc(encoded_size + 1U, 1U);
            CHECK(copy);
            memcpy(copy, encoded, encoded_size);
            char *end = strstr(copy, "\n\n");
            CHECK(end);
            at = (size_t)(end - copy) + 2U;
            free(copy);
        }
        encoded[at] = strcmp(mode, "magic") == 0 ? 'X' : '\0';
        CHECK(UmiDocumentRecoveryDraftDecode(encoded, encoded_size, NULL, &decoded) != UMI_STATUS_OK &&
              decoded == NULL);
    }
    if (strcmp(mode, "header-long") == 0)
    {
        char record[256];
        memset(record, 'x', sizeof(record));
        record[255] = '\n';
        CHECK(UmiDocumentRecoveryDraftDecode(record, sizeof(record), NULL, &decoded) != UMI_STATUS_OK &&
              decoded == NULL);
    }
    if (strcmp(mode, "key-generate") == 0)
    {
        char key[33];
        CHECK(UmiDocumentRecoveryKeyCreate(key, sizeof(key)) == UMI_STATUS_OK && strlen(key) == 32U);
        for (size_t i = 0U; i < 32U; ++i)
            CHECK((key[i] >= '0' && key[i] <= '9') || (key[i] >= 'a' && key[i] <= 'f'));
    }
    if (strcmp(mode, "key-capacity") == 0)
    {
        char key[4] = "old";
        CHECK(UmiDocumentRecoveryKeyCreate(key, sizeof(key)) == UMI_STATUS_CAPACITY_EXCEEDED &&
              key[0] == '\0');
    }
    if (strcmp(mode, "key-null") == 0)
        CHECK(UmiDocumentRecoveryKeyCreate(NULL, 33U) == UMI_STATUS_INVALID_ARGUMENT);
    UmiDocumentRecoveryDraftDestroy(draft);
    UmiDocumentRecoveryBytesFree(encoded);
    free(owned);
    return 0;
}
