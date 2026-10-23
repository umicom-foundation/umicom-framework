/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_encoding_boundaries.c
 *
 * PURPOSE:
 *   Verify deterministic C23 native-Web text/attribute escaping and bounded
 *   buffer failure status at the public Framework API boundary.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/websocket_patch_protocol.h"
#include "umicom/frontend/native_web/sse_patch_protocol.h"
#include "umicom/frontend/native_web/polling_patch_protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
typedef UmiStatus (*Encoder)(const UmiNativeWebPatchBatch *, UmiNativeWebMarkupBuffer *);
static Encoder encoders[] = {umi_native_web_websocket_patch_protocol_encode,
    umi_native_web_sse_patch_protocol_encode, umi_native_web_polling_patch_protocol_encode};
static void Reset(UmiNativeWebMarkupBuffer *out, UmiNativeWebMarkupBuffer *saved)
{ memset(out, 0x4d, sizeof *out); memcpy(saved, out, sizeof *out); }
static void Patch(UmiNativeWebPatch *patch)
{
    memset(patch, 0, sizeof *patch); patch->kind = UMI_NATIVE_WEB_PATCH_SET_TEXT;
    strcpy(patch->target_id, "x"); strcpy(patch->value, "hello"); patch->revision = 1U;
}
/* The historical entry point required a case argument. Keep it for review;
 * the aggregate CTest registration now runs every case instead of failing
 * before any boundary is checked. Explicit per-case invocation is unchanged. */
#if 0
int main(int argc, char **argv)
{
    UmiNativeWebPatchBatch *batch = calloc(1U, sizeof *batch);
    UmiNativeWebMarkupBuffer out, saved;
    CHECK(argc == 2 && batch != NULL); batch->count = 1U; batch->revision = 1U; Patch(&batch->items[0]);
    Reset(&out, &saved);
    if (!strcmp(argv[1], "buffer-state")) {
        umi_native_web_markup_buffer_init(&out); strcpy(out.data, "safe"); out.length = SIZE_MAX;
        CHECK(umi_native_web_markup_buffer_append_n(&out, "x", 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(out.length == SIZE_MAX && !strcmp(out.data, "safe"));
        umi_native_web_markup_buffer_init(&out); out.data[0] = 'x';
        CHECK(umi_native_web_markup_buffer_append_n(&out, "y", 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(out.length == 0U && out.data[0] == 'x');
    } else if (!strcmp(argv[1], "buffer-boundary")) {
        umi_native_web_markup_buffer_init(&out); memset(out.data, 'x', sizeof out.data - 1U);
        out.length = sizeof out.data - 1U; out.data[out.length] = 0;
        OK(umi_native_web_markup_buffer_append_n(&out, "", 0U));
        CHECK(umi_native_web_markup_buffer_append_n(&out, "y", 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(out.length == sizeof out.data - 1U && out.data[out.length] == 0);
        CHECK(umi_native_web_markup_buffer_append_n(&out, "", 0U) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (!strcmp(argv[1], "buffer-alias")) {
        umi_native_web_markup_buffer_init(&out); OK(umi_native_web_markup_buffer_append(&out, "abc"));
        OK(umi_native_web_markup_buffer_append_n(&out, out.data, 3U)); CHECK(!strcmp(out.data, "abcabc"));
    } else if (!strcmp(argv[1], "batch-count")) {
        batch->count = UMI_NATIVE_WEB_MAX_PATCHES + 1U;
        CHECK(umi_native_web_patch_serialize(batch, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&saved, &out, sizeof out) == 0);
    } else if (!strcmp(argv[1], "inline-string")) {
        memset(batch->items[0].value, 'x', sizeof batch->items[0].value);
        CHECK(umi_native_web_patch_serialize(batch, &out) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&saved, &out, sizeof out) == 0);
        Patch(&batch->items[0]); memset(batch->items[0].target_id, 'x', sizeof batch->items[0].target_id);
        CHECK(umi_native_web_patch_serialize(batch, &out) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&saved, &out, sizeof out) == 0);
    } else if (!strcmp(argv[1], "unicode")) {
        const unsigned char invalid[][5] = {{0xc0,0xaf,0},{0xed,0xa0,0x80,0},{0xf4,0x90,0x80,0x80,0},{0xe2,0x82,0}};
        for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i) {
            memcpy(batch->items[0].value, invalid[i], sizeof invalid[i]);
            CHECK(umi_native_web_patch_serialize(batch, &out) == UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(&saved, &out, sizeof out) == 0);
        }
        strcpy(batch->items[0].value, "\xe2\x82\xac");
        OK(umi_native_web_patch_serialize(batch, &out)); CHECK(strstr(out.data, "\xe2\x82\xac") != NULL);
    } else if (!strcmp(argv[1], "controls")) {
        strcpy(batch->items[0].value, "a\nb\tc\r\"\\");
        OK(umi_native_web_patch_serialize(batch, &out));
        CHECK(strstr(out.data, "\\u000a") != NULL && strstr(out.data, "\\u0009") != NULL);
        CHECK(strstr(out.data, "\\u000d") != NULL && strstr(out.data, "\\\"") != NULL);
        CHECK(strchr(out.data, '\n') == NULL && strchr(out.data, '\t') == NULL);
    } else if (!strcmp(argv[1], "atomic")) {
        batch->count = 2U; Patch(&batch->items[1]); batch->items[1].kind = (UmiNativeWebPatchKind)99;
        CHECK(umi_native_web_patch_serialize(batch, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&saved, &out, sizeof out) == 0);
        for (size_t i = 0U; i < sizeof encoders / sizeof encoders[0]; ++i) {
            CHECK(encoders[i](batch, &out) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&saved, &out, sizeof out) == 0);
        }
    } else if (!strcmp(argv[1], "frame-capacity")) {
        size_t remaining; batch->count = 16U;
        for (size_t i = 0U; i < batch->count; ++i) { Patch(&batch->items[i]); strcpy(batch->items[i].value, "x"); }
        OK(umi_native_web_patch_serialize(batch, &out));
        remaining = UMI_NATIVE_WEB_MARKUP_CAPACITY - 4U - out.length;
        for (size_t i = 0U; i < batch->count && remaining != 0U; ++i) {
            size_t extra = remaining < UMI_NATIVE_WEB_VALUE_CAPACITY - 2U ? remaining : UMI_NATIVE_WEB_VALUE_CAPACITY - 2U;
            memset(batch->items[i].value, 'x', extra + 1U); batch->items[i].value[extra + 1U] = 0; remaining -= extra;
        }
        CHECK(remaining == 0U); OK(umi_native_web_patch_serialize(batch, &out));
        CHECK(out.length == UMI_NATIVE_WEB_MARKUP_CAPACITY - 4U);
        Reset(&out, &saved);
        for (size_t i = 0U; i < sizeof encoders / sizeof encoders[0]; ++i) {
            CHECK(encoders[i](batch, &out) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(memcmp(&saved, &out, sizeof out) == 0);
        }
    } else if (!strcmp(argv[1], "wire-format")) {
        batch->count = 0U;
        OK(encoders[0](batch, &out)); CHECK(!strcmp(out.data, "PATCH []"));
        OK(encoders[1](batch, &out)); CHECK(!strcmp(out.data, "event: umicom-patch\ndata: []\n\n"));
        OK(encoders[2](batch, &out)); CHECK(!strcmp(out.data, "{\"patches\":[]}"));
    } else return 2;
    free(batch); return 0;
}
#endif
static int RunEncodingCase(int argc, char **argv)
{
    UmiNativeWebPatchBatch *batch = calloc(1U, sizeof *batch);
    UmiNativeWebMarkupBuffer out, saved;
    CHECK(argc == 2 && batch != NULL); batch->count = 1U; batch->revision = 1U; Patch(&batch->items[0]);
    Reset(&out, &saved);
    if (!strcmp(argv[1], "buffer-state")) {
        umi_native_web_markup_buffer_init(&out); strcpy(out.data, "safe"); out.length = SIZE_MAX;
        CHECK(umi_native_web_markup_buffer_append_n(&out, "x", 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(out.length == SIZE_MAX && !strcmp(out.data, "safe"));
        umi_native_web_markup_buffer_init(&out); out.data[0] = 'x';
        CHECK(umi_native_web_markup_buffer_append_n(&out, "y", 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(out.length == 0U && out.data[0] == 'x');
    } else if (!strcmp(argv[1], "buffer-boundary")) {
        umi_native_web_markup_buffer_init(&out); memset(out.data, 'x', sizeof out.data - 1U);
        out.length = sizeof out.data - 1U; out.data[out.length] = 0;
        OK(umi_native_web_markup_buffer_append_n(&out, "", 0U));
        CHECK(umi_native_web_markup_buffer_append_n(&out, "y", 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(out.length == sizeof out.data - 1U && out.data[out.length] == 0);
        CHECK(umi_native_web_markup_buffer_append_n(&out, "", 0U) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (!strcmp(argv[1], "buffer-alias")) {
        umi_native_web_markup_buffer_init(&out); OK(umi_native_web_markup_buffer_append(&out, "abc"));
        OK(umi_native_web_markup_buffer_append_n(&out, out.data, 3U)); CHECK(!strcmp(out.data, "abcabc"));
    } else if (!strcmp(argv[1], "batch-count")) {
        batch->count = UMI_NATIVE_WEB_MAX_PATCHES + 1U;
        CHECK(umi_native_web_patch_serialize(batch, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&saved, &out, sizeof out) == 0);
    } else if (!strcmp(argv[1], "inline-string")) {
        memset(batch->items[0].value, 'x', sizeof batch->items[0].value);
        CHECK(umi_native_web_patch_serialize(batch, &out) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&saved, &out, sizeof out) == 0);
        Patch(&batch->items[0]); memset(batch->items[0].target_id, 'x', sizeof batch->items[0].target_id);
        CHECK(umi_native_web_patch_serialize(batch, &out) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&saved, &out, sizeof out) == 0);
    } else if (!strcmp(argv[1], "unicode")) {
        const unsigned char invalid[][5] = {{0xc0,0xaf,0},{0xed,0xa0,0x80,0},{0xf4,0x90,0x80,0x80,0},{0xe2,0x82,0}};
        for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i) {
            memcpy(batch->items[0].value, invalid[i], sizeof invalid[i]);
            CHECK(umi_native_web_patch_serialize(batch, &out) == UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(&saved, &out, sizeof out) == 0);
        }
        strcpy(batch->items[0].value, "\xe2\x82\xac");
        OK(umi_native_web_patch_serialize(batch, &out)); CHECK(strstr(out.data, "\xe2\x82\xac") != NULL);
    } else if (!strcmp(argv[1], "controls")) {
        strcpy(batch->items[0].value, "a\nb\tc\r\"\\");
        OK(umi_native_web_patch_serialize(batch, &out));
        CHECK(strstr(out.data, "\\u000a") != NULL && strstr(out.data, "\\u0009") != NULL);
        CHECK(strstr(out.data, "\\u000d") != NULL && strstr(out.data, "\\\"") != NULL);
        CHECK(strchr(out.data, '\n') == NULL && strchr(out.data, '\t') == NULL);
    } else if (!strcmp(argv[1], "atomic")) {
        batch->count = 2U; Patch(&batch->items[1]); batch->items[1].kind = (UmiNativeWebPatchKind)99;
        CHECK(umi_native_web_patch_serialize(batch, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&saved, &out, sizeof out) == 0);
        for (size_t i = 0U; i < sizeof encoders / sizeof encoders[0]; ++i) {
            CHECK(encoders[i](batch, &out) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&saved, &out, sizeof out) == 0);
        }
    } else if (!strcmp(argv[1], "frame-capacity")) {
        size_t remaining; batch->count = 16U;
        for (size_t i = 0U; i < batch->count; ++i) { Patch(&batch->items[i]); strcpy(batch->items[i].value, "x"); }
        OK(umi_native_web_patch_serialize(batch, &out));
        remaining = UMI_NATIVE_WEB_MARKUP_CAPACITY - 4U - out.length;
        for (size_t i = 0U; i < batch->count && remaining != 0U; ++i) {
            size_t extra = remaining < UMI_NATIVE_WEB_VALUE_CAPACITY - 2U ? remaining : UMI_NATIVE_WEB_VALUE_CAPACITY - 2U;
            memset(batch->items[i].value, 'x', extra + 1U); batch->items[i].value[extra + 1U] = 0; remaining -= extra;
        }
        CHECK(remaining == 0U); OK(umi_native_web_patch_serialize(batch, &out));
        CHECK(out.length == UMI_NATIVE_WEB_MARKUP_CAPACITY - 4U);
        Reset(&out, &saved);
        for (size_t i = 0U; i < sizeof encoders / sizeof encoders[0]; ++i) {
            CHECK(encoders[i](batch, &out) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(memcmp(&saved, &out, sizeof out) == 0);
        }
    } else if (!strcmp(argv[1], "wire-format")) {
        batch->count = 0U;
        OK(encoders[0](batch, &out)); CHECK(!strcmp(out.data, "PATCH []"));
        OK(encoders[1](batch, &out)); CHECK(!strcmp(out.data, "event: umicom-patch\ndata: []\n\n"));
        OK(encoders[2](batch, &out)); CHECK(!strcmp(out.data, "{\"patches\":[]}"));
    } else return 2;
    free(batch); return 0;
}

int main(int argc, char **argv)
{
    if (argc != 1) return RunEncodingCase(argc, argv);
    char *cases[] = {"buffer-state", "buffer-boundary", "buffer-alias", "batch-count",
        "inline-string", "unicode", "controls", "atomic", "frame-capacity", "wire-format"};
    for (size_t index = 0U; index < sizeof cases / sizeof cases[0]; ++index) {
        char *arguments[] = {argv[0], cases[index], NULL};
        int status = RunEncodingCase(2, arguments);
        if (status != 0) return status;
    }
    return 0;
}
