/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/video_watermark.c
 *
 * PURPOSE:
 *   Compose an explicit SVG watermark without changing stream selection or exposing paths to filter syntax.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "video_render_internal.h"
#include <stdio.h>
#include <string.h>

/* Decoration works on an unpublished argument candidate. Existing callers keep
 * the original stream-copy APIs; Media opts in before handing a plan to a worker.
 * The only new external value remains one argv path, never filter syntax. */
UmiStatus UmiVideoRenderApplyWatermark(UmiVideoRenderPlan *plan, const char *svgPath)
{
    if (plan == NULL || svgPath == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(svgPath);
    if (status != UMI_STATUS_OK) return status;
    if (plan->watermarked) return UMI_STATUS_ALREADY_EXISTS;
    if (plan->count == 0U || plan->count > UMI_CHANNEL_MAX_ARGUMENTS ||
        memchr(plan->graph, 0, sizeof(plan->graph)) == NULL)
        return UMI_STATUS_INVALID_STATE;

    size_t inputs = 0U, inputEnd = 0U, videoMap = plan->count;
    bool graphPresent = false, codecPresent = false;
    for (size_t i = 0U; i < plan->count; ++i) {
        const char *arg = plan->arguments[i];
        if (arg == NULL) return UMI_STATUS_INVALID_STATE;
        if (strcmp(arg, "-i") == 0) {
            if (i + 1U >= plan->count || plan->arguments[i + 1U] == NULL)
                return UMI_STATUS_INVALID_STATE;
            ++inputs;
            inputEnd = i + 2U;
            ++i;
        } else if (strcmp(arg, "-map") == 0 || strcmp(arg, "-filter_complex") == 0 ||
                   strcmp(arg, "-c:v") == 0) {
            if (i + 1U >= plan->count || plan->arguments[i + 1U] == NULL)
                return UMI_STATUS_INVALID_STATE;
            if (strcmp(arg, "-map") == 0 && videoMap == plan->count) videoMap = i + 1U;
            if (strcmp(arg, "-filter_complex") == 0) graphPresent = true;
            if (strcmp(arg, "-c:v") == 0) codecPresent = true;
            ++i;
        }
    }
    if (inputs == 0U || inputs > UMI_VIDEO_SEQUENCE_MAX_CLIPS ||
        inputEnd > videoMap || videoMap == plan->count || !codecPresent ||
        graphPresent != (plan->graph[0] != '\0') ||
        strcmp(plan->arguments[videoMap], graphPresent ? "[out]" : "0:v:0") != 0)
        return UMI_STATUS_INVALID_STATE;

    /* Reference scaling fits wide and portrait videos without warping the
     * canonical artwork. Repeating its one decoded frame keeps it visible until
     * the primary video ends. No fonts, user text or paths enter this graph. */
    char graph[sizeof(plan->graph)];
    int length = snprintf(graph, sizeof(graph),
        "%s%s%ssplit=2[umi_base][umi_ref];"
        "[%zu:v:0]setpts=PTS-STARTPTS[umi_logo];"
        "[umi_logo][umi_ref]scale=w='max(1,trunc(min(rw/5,rh/5*dar)))':h=-1[umi_scaled];"
        "[umi_scaled]format=rgba,pad=iw+4:ih+4:2:2:color=0x0d1826@0.75[umi_mark];"
        "[umi_base][umi_mark]overlay=x=main_w-overlay_w-main_w/50:"
        "y=main_h-overlay_h-main_h/50:eof_action=repeat:repeatlast=1:shortest=0[umi_branded]",
        plan->graph, graphPresent ? ";" : "", graphPresent ? "[out]" : "[0:v:0]", inputs);
    if (length < 0 || (size_t)length >= sizeof(graph)) return UMI_STATUS_CAPACITY_EXCEEDED;
    char pathCopy[sizeof(plan->watermark_path)];
    size_t pathSize = strlen(svgPath) + 1U;
    if (pathSize > sizeof(pathCopy)) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(pathCopy, svgPath, pathSize);

    const char *args[UMI_CHANNEL_MAX_ARGUMENTS];
    size_t count = 0U;
#define ADD(value) do { \
    if (count == UMI_CHANNEL_MAX_ARGUMENTS) return UMI_STATUS_CAPACITY_EXCEEDED; \
    args[count++] = (value); \
} while (0)
    for (size_t i = 0U; i < inputEnd; ++i) ADD(plan->arguments[i]);
    /* Explicit format/decoder also work with a private extensionless temporary
     * file. Missing librsvg support is an encoder failure, never an unbranded
     * fallback. The file-only protocol policy remains scoped to each input. */
    ADD("-protocol_whitelist"); ADD("file");
    ADD("-f"); ADD("image2pipe");
    ADD("-c:v"); ADD("librsvg");
    ADD("-i"); ADD(plan->watermark_path);
    if (!graphPresent) { ADD("-filter_complex"); ADD(plan->graph); }
    for (size_t i = inputEnd; i < plan->count; ++i) {
        const char *arg = plan->arguments[i];
        /* Explicit -map entries already exclude unselected data/subtitles.
         * Retiring these redundant switches leaves all six sequence inputs
         * within the shared 64-argument process-channel limit. */
        if (strcmp(arg, "-sn") == 0 || strcmp(arg, "-dn") == 0) continue;
        if (i == videoMap) { ADD("[umi_branded]"); continue; }
        if (strcmp(arg, "-filter_complex") == 0) {
            ADD(arg); ADD(plan->graph); ++i; continue;
        }
        if (strcmp(arg, "-c:v") == 0) {
            ADD(arg); ADD("libx264");
            if (strcmp(plan->arguments[++i], "copy") == 0) {
                ADD("-threads"); ADD("2"); ADD("-pix_fmt"); ADD("yuv420p");
            }
            continue;
        }
        ADD(arg);
    }
#undef ADD
    /* Publish only after every bounded operation succeeds. Argument pointers
     * address stable storage in plan, never these stack candidates. */
    memcpy(plan->graph, graph, (size_t)length + 1U);
    memcpy(plan->watermark_path, pathCopy, pathSize);
    memcpy(plan->arguments, args, count * sizeof(args[0]));
    plan->count = count;
    plan->watermarked = true;
    return UMI_STATUS_OK;
}
