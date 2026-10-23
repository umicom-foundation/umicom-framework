/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_text_validation.c
 * PURPOSE: Reject unterminated inline diagnostic fields before shared model storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/diagnostics/model.h"
#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "%d: %s\n", __LINE__, #expr); result = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"unterminated-id", "boundary-id", "unterminated-source", "boundary-source", "unterminated-code", "boundary-code", "unterminated-message", "boundary-message", "unterminated-detail", "boundary-detail", "unterminated-uri", "boundary-uri", "unterminated-owner", "boundary-owner", "unterminated-tags", "boundary-tags", "all-terminated", "message-buffer", "encoding"};
    int known = 0, result = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(argv[1], cases[i]) == 0) known = 1;
    if (!known) return 2;
    const char *mode = argv[1];
    UmiDiagnosticSnapshot item, before, stored;
    UmiDiagnosticModel *model = NULL;
    char message[128];
    CHECK(umi_diagnostic_snapshot_init(&item, "original", UMI_DIAGNOSTIC_ERROR,
        UMI_DIAGNOSTIC_KIND_COMPILER, "cc", "original message") == UMI_STATUS_OK);
    CHECK(umi_diagnostic_model_create(NULL, &model) == UMI_STATUS_OK);
    CHECK(umi_diagnostic_model_upsert(model, &item) == UMI_STATUS_OK);
    uint64_t revision = umi_diagnostic_model_revision(model);
    struct Field { const char *name; char *text; size_t capacity; } fields[] = {
        {"id", item.id, sizeof item.id},
        {"source", item.source, sizeof item.source},
        {"code", item.code, sizeof item.code},
        {"message", item.message, sizeof item.message},
        {"detail", item.detail, sizeof item.detail},
        {"uri", item.uri, sizeof item.uri},
        {"owner", item.owner, sizeof item.owner},
        {"tags", item.tags, sizeof item.tags}
    };
    int unterminated = strncmp(mode, "unterminated-", 13U) == 0;
    if (unterminated || strncmp(mode, "boundary-", 9U) == 0) {
        const char *name = mode + (unterminated ? 13U : 9U);
        for (size_t i = 0U; i < sizeof fields / sizeof fields[0]; ++i) if (strcmp(name, fields[i].name) == 0) {
            memset(fields[i].text, 'x', fields[i].capacity);
            if (!unterminated) fields[i].text[fields[i].capacity - 1U] = '\0';
        }
    } else if (strcmp(mode, "all-terminated") == 0) {
        for (size_t i = 0U; i < sizeof fields / sizeof fields[0]; ++i) {
            memset(fields[i].text, 'x', fields[i].capacity);
            fields[i].text[fields[i].capacity - 1U] = '\0';
        }
    } else if (strcmp(mode, "encoding") == 0) {
        item.message[0] = (char)0xff; item.message[1] = '\0';
    }
    memcpy(&before, &item, sizeof before);
    UmiStatus expected = unterminated ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
    CHECK(umi_diagnostic_snapshot_validate(&item, message, sizeof message) == expected);
    CHECK(memcmp(&before, &item, sizeof before) == 0);
    CHECK(umi_diagnostic_model_upsert(model, &item) == expected);
    if (unterminated) {
        CHECK(strstr(message, "terminated") != NULL);
        CHECK(umi_diagnostic_model_revision(model) == revision && umi_diagnostic_model_count(model) == 1U);
        CHECK(umi_diagnostic_model_at(model, 0U, &stored) == UMI_STATUS_OK);
        CHECK(strcmp(stored.id, "original") == 0 && strcmp(stored.message, "original message") == 0);
    } else if (strcmp(mode, "message-buffer") == 0) {
        struct { char before, text[1], after; } buffer = {'a', {'x'}, 'b'};
        CHECK(umi_diagnostic_snapshot_validate(NULL, buffer.text, sizeof buffer.text) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(buffer.before == 'a' && buffer.text[0] == '\0' && buffer.after == 'b');
        CHECK(umi_diagnostic_snapshot_validate(&item, NULL, 0U) == UMI_STATUS_OK);
    } else if (strcmp(mode, "encoding") == 0) {
        CHECK(umi_diagnostic_model_at(model, 0U, &stored) == UMI_STATUS_OK);
        CHECK((unsigned char)stored.message[0] == 0xffU);
    }
done:
    umi_diagnostic_model_destroy(model);
    return result;
}
