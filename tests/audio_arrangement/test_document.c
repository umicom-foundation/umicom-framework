/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/audio_arrangement/test_document.c
 * PURPOSE: Reject ambiguous or incomplete audio recipes and preserve the previous value on failed import.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    const char *cases[] = {"roundtrip",     "empty",         "unicode",       "duplicate-key",
                           "unknown-field", "missing-field", "fractional",    "negative",
                           "overflow",      "duplicate-id",  "invalid-range", "trailing",
                           "cancelled",     "malformed",     "live-export",   "over-capacity"};
    if (argc != 2 || !Known(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
        return 2;
    const char *mode = argv[1];
    UmiCreativeAudioArrangement plan = Plan(), out = plan;
    UmiCreativeAsset *asset = NULL;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "empty") == 0)
        plan.clip_count = 0U;
    if (strcmp(mode, "unicode") == 0)
        strcpy(plan.title, "caf\xc3\xa9 \"mix\"");
    if (strcmp(mode, "live-export") == 0)
    {
        asset = Capture("keep");
        UmiCreativeAsset *same = asset;
        CHECK(UmiCreativeAudioArrangementExport(&plan, NULL, &asset) == UMI_STATUS_INVALID_STATE &&
              asset == same);
        UmiCreativeAssetDestroy(asset);
        asset = NULL;
    }
    CHECK(UmiCreativeAudioArrangementExport(&plan, NULL, &asset) == UMI_STATUS_OK);
    const void *encoded = NULL;
    size_t size = 0U;
    CHECK(UmiCreativeAssetBytes(asset, &encoded, &size) == UMI_STATUS_OK);
    char *bytes = malloc(size + 256U);
    CHECK(bytes != NULL);
    memcpy(bytes, encoded, size);
    bytes[size] = '\0';
    bool valid = true;
    if (strcmp(mode, "duplicate-key") == 0 || strcmp(mode, "unknown-field") == 0)
    {
        const char *extra = strcmp(mode, "duplicate-key") == 0 ? "\"title\":\"other\"," : "\"unknown\":true,";
        memmove(bytes + 1U + strlen(extra), bytes + 1U, size);
        memcpy(bytes + 1U, extra, strlen(extra));
        size += strlen(extra);
        valid = false;
    }
    else if (strcmp(mode, "missing-field") == 0)
    {
        char *at = strstr(bytes, "sample_rate");
        CHECK(at != NULL);
        memcpy(at, "missing_key", 11U);
        valid = false;
    }
    else if (strcmp(mode, "fractional") == 0 || strcmp(mode, "negative") == 0)
    {
        char *at = strstr(bytes, "8000");
        CHECK(at != NULL);
        memcpy(at, strcmp(mode, "fractional") == 0 ? "8e03" : "-100", 4U);
        valid = false;
    }
    else if (strcmp(mode, "overflow") == 0)
    {
        strcpy(bytes, "{\"format\":\"umicom-audio-arrangement\",\"title\":\"Mix\",\"sample_rate\":4294967296,"
                      "\"clips\":[]}");
        size = strlen(bytes);
        valid = false;
    }
    else if (strcmp(mode, "invalid-range") == 0)
    {
        char *at = strstr(bytes, "\"source_end_ms\":16");
        CHECK(at != NULL);
        at += strlen("\"source_end_ms\":");
        memcpy(at, " 0", 2U);
        valid = false;
    }
    else if (strcmp(mode, "duplicate-id") == 0)
    {
        UmiCreativeAudioArrangement two = plan;
        two.clips[1] = two.clips[0];
        strcpy(two.clips[1].id, "other");
        two.clip_count = 2U;
        UmiCreativeAsset *both = NULL;
        CHECK(UmiCreativeAudioArrangementExport(&two, NULL, &both) == UMI_STATUS_OK);
        const void *data = NULL;
        size_t count = 0U;
        CHECK(UmiCreativeAssetBytes(both, &data, &count) == UMI_STATUS_OK);
        free(bytes);
        bytes = malloc(count + 1U);
        CHECK(bytes != NULL);
        memcpy(bytes, data, count);
        bytes[count] = '\0';
        size = count;
        char *at = strstr(bytes, "other");
        CHECK(at != NULL);
        memcpy(at, "first", 5U);
        UmiCreativeAssetDestroy(both);
        valid = false;
    }
    else if (strcmp(mode, "trailing") == 0)
    {
        bytes[size++] = 'x';
        valid = false;
    }
    else if (strcmp(mode, "malformed") == 0)
    {
        bytes[0] = '[';
        valid = false;
    }
    else if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        valid = false;
    }
    else if (strcmp(mode, "over-capacity") == 0)
    {
        UmiCreativeAudioArrangement too_many = plan;
        too_many.clip_count = UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_CLIPS + 1U;
        UmiCreativeAsset *none = NULL;
        CHECK(UmiCreativeAudioArrangementExport(&too_many, NULL, &none) == UMI_STATUS_CAPACITY_EXCEEDED &&
              none == NULL);
    }
    UmiCreativeAudioArrangement previous = out;
    UmiStatus status = UmiCreativeAudioArrangementImport(bytes, size, cancel, &out);
    if (valid)
        CHECK(status == UMI_STATUS_OK && out.sample_rate == plan.sample_rate &&
              out.clip_count == plan.clip_count && strcmp(out.title, plan.title) == 0);
    else
        CHECK(status != UMI_STATUS_OK && memcmp(&out, &previous, sizeof(out)) == 0);
    free(bytes);
    UmiCreativeAssetDestroy(asset);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
