/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/audio_arrangement/test_render.c
 * PURPOSE: Check audible sample values, complete failure semantics and timed source selection independently of the mixer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    const char *cases[] = {"mono",
                           "stereo",
                           "gap",
                           "overlap",
                           "signed-clipping",
                           "cancellation-before-saturation",
                           "attenuation",
                           "silence",
                           "trim",
                           "trim-ramp",
                           "negative-clipping",
                           "fades",
                           "negative-fades",
                           "source-reuse",
                           "sample-rate",
                           "missing-source",
                           "invalid-wave",
                           "outside-source",
                           "output-limit",
                           "cancelled",
                           "live-output",
                           "empty-plan",
                           "invalid-plan",
                           "duplicate-placement",
                           "explicit-replace",
                           "remove"};
    if (argc != 2 || !Known(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
        return 2;
    const char *mode = argv[1];
    bool stereo = strcmp(mode, "stereo") == 0;
    int16_t magnitude = strcmp(mode, "negative-clipping") == 0                ? -30000
                        : strcmp(mode, "signed-clipping") == 0                ? 30000
                        : strcmp(mode, "cancellation-before-saturation") == 0 ? 20000
                        : strcmp(mode, "negative-fades") == 0                 ? -16000
                                                                              : 16000;
    UmiCreativeAssetLibrary *library = Sources(strcmp(mode, "output-limit") == 0  ? 48000U
                                               : strcmp(mode, "sample-rate") == 0 ? 16000U
                                                                                  : 8000U,
                                               stereo ? 2U : 1U, magnitude, -12000);
    UmiCreativeAudioArrangement plan = Plan(), before = plan;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "gap") == 0)
        plan.clips[0].start_ms = 4U;
    if (strcmp(mode, "attenuation") == 0)
        plan.clips[0].gain_permille = 250U;
    if (strcmp(mode, "silence") == 0)
        plan.clips[0].gain_permille = 0U;
    if (strcmp(mode, "trim") == 0)
    {
        plan.clips[0].source_begin_ms = 4U;
        plan.clips[0].source_end_ms = 12U;
    }
    if (strcmp(mode, "fades") == 0 || strcmp(mode, "negative-fades") == 0)
    {
        plan.clips[0].fade_in_ms = 4U;
        plan.clips[0].fade_out_ms = 4U;
    }
    if (strcmp(mode, "overlap") == 0 || strcmp(mode, "source-reuse") == 0 ||
        strcmp(mode, "signed-clipping") == 0 || strcmp(mode, "negative-clipping") == 0 ||
        strcmp(mode, "cancellation-before-saturation") == 0)
    {
        UmiCreativeAudioPlacement second = plan.clips[0];
        strcpy(second.id, "second");
        CHECK(UmiCreativeAudioArrangementPut(&plan, &second, false) == UMI_STATUS_OK);
        if (strcmp(mode, "cancellation-before-saturation") == 0)
        {
            UmiCreativeAsset *negative = Wave(8000U, 1U, -20000, -20000);
            CHECK(UmiCreativeAssetLibraryInsert(library, "negative", &negative) == UMI_STATUS_OK);
            UmiCreativeAudioPlacement third = plan.clips[0];
            strcpy(third.id, "third");
            strcpy(third.asset_id, "negative");
            CHECK(UmiCreativeAudioArrangementPut(&plan, &third, false) == UMI_STATUS_OK);
            /* The positive intermediate sum would clip if saturation happened
             * before the negative contribution, but the final mix does not. */
            plan.clips[0].gain_permille = 1000U;
            plan.clips[1].gain_permille = 1000U;
        }
    }
    if (strcmp(mode, "trim-ramp") == 0)
    {
        UmiCreativeAsset *constant = Wave(8000U, 1U, 0, 0), *ramp = NULL;
        const void *source = NULL;
        size_t size = 0U;
        CHECK(UmiCreativeAssetBytes(constant, &source, &size) == UMI_STATUS_OK);
        unsigned char *bytes = malloc(size);
        CHECK(bytes != NULL);
        memcpy(bytes, source, size);
        for (size_t i = 0U; i < 128U; ++i)
            Put16(bytes + 44U + i * 2U, (uint16_t)(i * 100U));
        CHECK(UmiCreativeAssetCapture("ramp", UMI_CREATIVE_ASSET_AUDIO, bytes, size, NULL, &ramp) ==
              UMI_STATUS_OK);
        CHECK(UmiCreativeAssetLibraryInsert(library, "ramp", &ramp) == UMI_STATUS_OK);
        UmiCreativeAssetDestroy(constant);
        free(bytes);
        strcpy(plan.clips[0].asset_id, "ramp");
        plan.clips[0].source_begin_ms = 4U;
        plan.clips[0].source_end_ms = 12U;
    }
    if (strcmp(mode, "sample-rate") == 0 || strcmp(mode, "outside-source") == 0 ||
        strcmp(mode, "invalid-plan") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "outside-source") == 0)
        plan.clips[0].source_end_ms = 17U;
    if (strcmp(mode, "invalid-plan") == 0)
        plan.clips[0].gain_permille = 1001U;
    if (strcmp(mode, "missing-source") == 0)
    {
        strcpy(plan.clips[0].asset_id, "absent");
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "invalid-wave") == 0)
    {
        UmiCreativeAsset *bad = Capture("not a WAV");
        CHECK(UmiCreativeAssetLibraryInsert(library, "bad", &bad) == UMI_STATUS_OK);
        strcpy(plan.clips[0].asset_id, "bad");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "output-limit") == 0)
    {
        plan.sample_rate = 48000U;
        plan.clips[0].source_end_ms = 2U;
        plan.clips[0].start_ms = 599000U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "empty-plan") == 0)
    {
        plan.clip_count = 0U;
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "duplicate-placement") == 0)
    {
        CHECK(UmiCreativeAudioArrangementPut(&plan, &before.clips[0], false) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(memcmp(&plan, &before, sizeof(plan)) == 0);
    }
    if (strcmp(mode, "explicit-replace") == 0)
    {
        UmiCreativeAudioPlacement replacement = before.clips[0];
        replacement.gain_permille = 500U;
        CHECK(UmiCreativeAudioArrangementPut(&plan, &replacement, true) == UMI_STATUS_OK);
        CHECK(plan.clip_count == 1U && plan.clips[0].gain_permille == 500U);
        strcpy(replacement.id, "unknown");
        CHECK(UmiCreativeAudioArrangementPut(&plan, &replacement, true) == UMI_STATUS_NOT_FOUND);
    }
    if (strcmp(mode, "remove") == 0)
    {
        CHECK(UmiCreativeAudioArrangementRemove(&plan, "missing") == UMI_STATUS_NOT_FOUND);
        CHECK(UmiCreativeAudioArrangementRemove(&plan, "first") == UMI_STATUS_OK && plan.clip_count == 0U);
        expected = UMI_STATUS_INVALID_STATE;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiCreativeAsset *mixed = NULL;
    UmiCreativeAudioArrangementReport report = {99U, 99U, 99U, 99U, 99U};
    UmiCreativeAudioArrangementReport prior = report;
    if (strcmp(mode, "live-output") == 0)
    {
        mixed = Capture("keep");
        UmiCreativeAsset *same = mixed;
        CHECK(UmiCreativeAudioArrangementRender(&plan, library, cancel, &mixed, &report) ==
                  UMI_STATUS_INVALID_STATE &&
              mixed == same);
        CHECK(memcmp(&prior, &report, sizeof(report)) == 0);
        UmiCreativeAssetDestroy(mixed);
        mixed = NULL;
    }
    CHECK(UmiCreativeAudioArrangementRender(&plan, library, cancel, &mixed, &report) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(mixed == NULL && memcmp(&prior, &report, sizeof(report)) == 0);
    else
    {
        CHECK(report.sample_rate == 8000U && report.placements == plan.clip_count);
        if (strcmp(mode, "gap") == 0)
            CHECK(report.frames == 160U && Sample(mixed, 0U, 0U) == 0 && Sample(mixed, 31U, 1U) == 0 &&
                  Sample(mixed, 32U, 0U) == 16000);
        else if (strcmp(mode, "trim") == 0)
            CHECK(report.frames == 64U && Sample(mixed, 63U, 1U) == 16000);
        else if (strcmp(mode, "trim-ramp") == 0)
            CHECK(report.frames == 64U && Sample(mixed, 0U, 0U) == 3200 && Sample(mixed, 63U, 1U) == 9500);
        else if (strcmp(mode, "negative-clipping") == 0)
            CHECK(report.clipped_samples == 256U && Sample(mixed, 0U, 0U) == -32768);
        else if (strcmp(mode, "signed-clipping") == 0)
            CHECK(report.clipped_samples == 256U && Sample(mixed, 0U, 0U) == 32767);
        else if (strcmp(mode, "overlap") == 0 || strcmp(mode, "source-reuse") == 0)
            CHECK(report.unique_sources == 1U && Sample(mixed, 0U, 0U) == 32000 &&
                  report.clipped_samples == 0U);
        else if (strcmp(mode, "cancellation-before-saturation") == 0)
            CHECK(Sample(mixed, 0U, 0U) == 20000 && report.clipped_samples == 0U &&
                  report.unique_sources == 2U);
        else if (strcmp(mode, "attenuation") == 0)
            CHECK(Sample(mixed, 0U, 0U) == 4000);
        else if (strcmp(mode, "silence") == 0)
            CHECK(Sample(mixed, 127U, 1U) == 0);
        else if (strcmp(mode, "fades") == 0 || strcmp(mode, "negative-fades") == 0)
        {
            CHECK(Sample(mixed, 0U, 0U) == 0 && Sample(mixed, 127U, 1U) == 0);
            CHECK(Sample(mixed, 31U, 0U) == magnitude && Sample(mixed, 96U, 1U) == magnitude);
            CHECK(Sample(mixed, 1U, 0U) == (int16_t)(magnitude / 31));
        }
        else if (strcmp(mode, "explicit-replace") == 0)
            CHECK(Sample(mixed, 0U, 0U) == 8000);
        else
            CHECK(Sample(mixed, 0U, 0U) == 16000 && Sample(mixed, 0U, 1U) == (stereo ? -12000 : 16000));
    }
    UmiCreativeAssetDestroy(mixed);
    UmiCreativeAssetLibraryDestroy(library);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
