/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/audio_arrangement/command.c
 * PURPOSE: Expose the shared arrangement renderer to explicit Studio tasks and native shell sessions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "command.h"
#include "umicom/creative_workspace/audio_arrangement.h"
#include "umicom/creative_workspace/asset_library_archive.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
static UmiStatus ReadPlan(const char *path, UmiCreativeAudioArrangement *out)
{
    UmiCreativeAsset *document = NULL;
    const void *bytes = NULL;
    size_t size = 0U;
    UmiStatus status =
        UmiCreativeAssetLoadFile(path, "Arrangement", UMI_CREATIVE_ASSET_DOCUMENT,
                                 UMI_CREATIVE_AUDIO_ARRANGEMENT_DOCUMENT_BYTES, NULL, &document);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetBytes(document, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAudioArrangementImport(bytes, size, NULL, out);
    UmiCreativeAssetDestroy(document);
    return status;
}
static int Finish(UmiStatus status)
{
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Arrangement operation failed: %s. Existing files are never overwritten.\n",
                umi_status_text(status));
    return status == UMI_STATUS_OK ? 0 : 1;
}
int UmiAudioArrangementCommand(int argc, char **argv)
{
    if (argc < 1 || argv == NULL)
        return 2;
    for (int i = 0; i < argc; ++i)
        if (argv[i] == NULL)
            return 2;
    if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        puts("Umicom audio arrangement\n"
             "  inspect ABSOLUTE_ARRANGEMENT.json\n"
             "  render ABSOLUTE_LIBRARY.umilibrary ABSOLUTE_ARRANGEMENT.json ABSOLUTE_NEW.wav\n"
             "Inspect reads the recipe only. Render validates all sources, mixes in memory, then creates a "
             "new WAV.\n"
             "PCM16 WAV sources: mono/stereo, <=4 MiB each, matching sample rates. No playback or "
             "resampling.\n"
             "Paths must be absolute. Quote each path containing spaces. --help performs no file I/O.");
        return 0;
    }
    if (argc == 3 && strcmp(argv[1], "inspect") == 0)
    {
        UmiCreativeAudioArrangement plan;
        UmiStatus status = ReadPlan(argv[2], &plan);
        if (status == UMI_STATUS_OK)
        {
            printf("%s: %u Hz, %zu placement(s). Source library has not been loaded or validated.\n",
                   plan.title, plan.sample_rate, plan.clip_count);
            for (size_t i = 0U; i < plan.clip_count; ++i)
            {
                const UmiCreativeAudioPlacement *clip = &plan.clips[i];
                printf("%s: asset %s, timeline %u ms, source [%u,%u) ms, gain %u/1000, fades %u/%u ms\n",
                       clip->id, clip->asset_id, clip->start_ms, clip->source_begin_ms, clip->source_end_ms,
                       clip->gain_permille, clip->fade_in_ms, clip->fade_out_ms);
            }
        }
        return Finish(status);
    }
    if (argc == 5 && strcmp(argv[1], "render") == 0)
    {
        /* Refuse unsuitable output syntax before reading inputs. The shared
         * writer still performs the exclusive creation check at publication. */
        UmiStatus status = UmiOutputFileValidatePath(argv[4]);
        UmiCreativeAudioArrangement plan;
        UmiCreativeAssetLibrary *library = NULL;
        UmiCreativeAsset *rendered = NULL;
        UmiCreativeAudioArrangementReport report = {0};
        UmiCreativeAssetWriteResult written = {0};
        if (status == UMI_STATUS_OK)
            status = ReadPlan(argv[3], &plan);
        if (status == UMI_STATUS_OK)
            status = UmiCreativeAssetLibraryLoad(argv[2], UMI_CREATIVE_LIBRARY_MAX_BYTES, NULL, &library);
        if (status == UMI_STATUS_OK)
            status = UmiCreativeAudioArrangementRender(&plan, library, NULL, &rendered, &report);
        if (status == UMI_STATUS_OK)
        {
            printf("Rendered %" PRIu64 " stereo frames at %u Hz; %zu placement(s), %zu source(s), %" PRIu64
                   " clipped sample(s).\n",
                   report.frames, report.sample_rate, report.placements, report.unique_sources,
                   report.clipped_samples);
            if (report.clipped_samples != 0U)
                fputs("Reduce placement gains to avoid clipping in another render.\n", stderr);
            status = UmiCreativeAssetWriteNew(rendered, argv[4], NULL, &written);
        }
        if (written.created)
            printf("New file retained: %s (%" PRIu64 " bytes written). Result: %s.\n", written.file.path,
                   written.file.bytes_written, umi_status_text(status));
        UmiCreativeAssetDestroy(rendered);
        UmiCreativeAssetLibraryDestroy(library);
        return Finish(status);
    }
    fputs("Use --help, inspect with one path, or render with library, arrangement and new WAV paths.\n",
          stderr);
    return 2;
}
