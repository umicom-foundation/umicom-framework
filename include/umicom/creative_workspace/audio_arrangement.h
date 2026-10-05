/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/audio_arrangement.h
 * PURPOSE: Describe and render timed PCM audio arrangements from a captured asset library.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_AUDIO_ARRANGEMENT_H
#define UMICOM_CREATIVE_WORKSPACE_AUDIO_ARRANGEMENT_H
#include "umicom/creative_workspace/asset_library.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_CLIPS 16U
#define UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_TIME_MS 600000U
#define UMI_CREATIVE_AUDIO_ARRANGEMENT_DOCUMENT_BYTES 16384U
    typedef struct UmiCreativeAudioPlacement
    {
        char id[UMI_CREATIVE_ID_CAPACITY];
        char asset_id[UMI_CREATIVE_ID_CAPACITY];
        uint32_t start_ms, source_begin_ms, source_end_ms;
        unsigned gain_permille;
        uint32_t fade_in_ms, fade_out_ms;
    } UmiCreativeAudioPlacement;
    /* Caller-owned value. Placements reference library IDs, not paths. Editing a
 * plan does not mutate its assets, save files or access an audio device. */
    typedef struct UmiCreativeAudioArrangement
    {
        char title[UMI_CREATIVE_LABEL_CAPACITY];
        uint32_t sample_rate;
        size_t clip_count;
        UmiCreativeAudioPlacement clips[UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_CLIPS];
    } UmiCreativeAudioArrangement;
    typedef struct UmiCreativeAudioArrangementReport
    {
        uint32_t sample_rate;
        uint64_t frames, clipped_samples;
        size_t placements, unique_sources;
    } UmiCreativeAudioArrangementReport;
    /* Init and edits leave output unchanged on failure. Rates are 8000..192000 Hz;
 * gain is 0..1000 (attenuation only). Source ranges are nonempty half-open
 * millisecond intervals, and each fade fits within its selected range. */
    UmiStatus UmiCreativeAudioArrangementInit(const char *title, uint32_t sample_rate,
                                              UmiCreativeAudioArrangement *out);
    UmiStatus UmiCreativeAudioArrangementValidate(const UmiCreativeAudioArrangement *plan);
    UmiStatus UmiCreativeAudioArrangementPut(UmiCreativeAudioArrangement *plan,
                                             const UmiCreativeAudioPlacement *placement,
                                             bool replace_existing);
    UmiStatus UmiCreativeAudioArrangementRemove(UmiCreativeAudioArrangement *plan, const char *id);
    /* All sources must be PCM16 WAVE accepted by UmiCreativeAudioDecode (up to 4 MiB
 * per source, mono/stereo). Source sample rates must equal the plan rate: no
 * implicit resampling. Output is stereo PCM16. Mono is copied to both channels.
 * Time values convert to frames by floor(ms*rate/1000). Source-end frames must
 * fit the captured clip. Empty plans are refused for rendering.
 * Gains/fades use the same integer order as single-clip edits; clips then add
 * in a wide accumulator. Saturation happens once per final sample and is
 * reported. Reduce gains when clipped_samples is nonzero. Silence fills gaps.
 * Output is at most 64 MiB including WAVE framing. No I/O or playback occurs.
 * Call on a worker, keeping the library unchanged. Cancel checks occur between
 * source decodes and bounded rendering chunks. Initialise *out=NULL; failure
 * preserves both outputs. The returned capture owns the complete rendered file. */
    UmiStatus UmiCreativeAudioArrangementRender(const UmiCreativeAudioArrangement *plan,
                                                const UmiCreativeAssetLibrary *library,
                                                const UmiCancellationToken *cancel, UmiCreativeAsset **out,
                                                UmiCreativeAudioArrangementReport *report);
    /* Portable JSON stores only editable metadata and IDs. Import validates the
 * complete document before publishing a value. It does not resolve sources.
 * Export returns a document capture suitable for explicit new-file storage. */
    UmiStatus UmiCreativeAudioArrangementExport(const UmiCreativeAudioArrangement *plan,
                                                const UmiCancellationToken *cancel, UmiCreativeAsset **out);
    UmiStatus UmiCreativeAudioArrangementImport(const void *bytes, size_t size,
                                                const UmiCancellationToken *cancel,
                                                UmiCreativeAudioArrangement *out);
#ifdef __cplusplus
}
#endif
#endif
