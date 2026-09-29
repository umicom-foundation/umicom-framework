/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Explicit file paths and frame boundaries. No automatic playback or overwrite. */
#include "lesson.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#include <string.h>
static bool Number(const char *text, uint64_t *out)
{
    if (text == NULL || *text == '\0') return false;
    uint64_t value = 0U;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c < '0' || c > '9' || value > (UINT64_MAX - (c - '0')) / 10U) return false;
        value = value * 10U + (c - '0');
    }
    *out = value; return true;
}
static int Finish(UmiStatus status)
{
    if (status != UMI_STATUS_OK) fprintf(stderr, "Audio operation failed: %s. No overwrite or automatic retry.\n", umi_status_text(status));
    return status == UMI_STATUS_OK ? 0 : 1;
}
static int CommandMain(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) return Finish(UmiCreativeAudioLesson(NULL));
    if (argc == 3 && strcmp(argv[1], "demo") == 0) {
        UmiCreativeExport output = {0};
        UmiStatus status = UmiCreativeAudioLesson(&output);
        if (status == UMI_STATUS_OK) status = UmiCreativeExportWriteNew(&output, argv[2]);
        UmiCreativeExportFree(&output); return Finish(status);
    }
    if (argc == 3 && strcmp(argv[1], "inspect") == 0) {
        UmiCreativeAudioClip *clip = NULL;
        UmiStatus status = UmiCreativeAudioLoadFile(argv[2], &clip);
        UmiCreativeAudioInfo info;
        UmiCreativeAudioOverview overview;
        if (status == UMI_STATUS_OK) status = UmiCreativeAudioGetInfo(clip, &info);
        if (status == UMI_STATUS_OK) status = UmiCreativeAudioInspect(clip, 0U, info.frames, 64U, &overview);
        if (status == UMI_STATUS_OK) {
            printf("PCM16: %u Hz, %u channel(s), %" PRIu64 " frames, %.6f seconds.\n",
                info.sampleRate, (unsigned)info.channels, info.frames, (double)info.frames / info.sampleRate);
            printf("Ignored metadata chunks: %zu. Metadata will not be preserved on export.\n", info.ignoredChunks);
            for (unsigned c = 0U; c < info.channels; ++c)
                printf("Channel %u: peak %u; RMS %.3f PCM units.\n", c + 1U, overview.peakMagnitude[c], overview.rms[c]);
        }
        UmiCreativeAudioDestroy(clip); return Finish(status);
    }
    if (argc == 9 && strcmp(argv[1], "trim") == 0) {
        uint64_t numbers[5];
        for (size_t i = 0U; i < 5U; ++i) if (!Number(argv[4U + i], &numbers[i])) return Finish(UMI_STATUS_INVALID_ARGUMENT);
        if (numbers[2] > 1000U) return Finish(UMI_STATUS_INVALID_ARGUMENT);
        UmiCreativeAudioEdit edit = {numbers[0], numbers[1], (unsigned)numbers[2], numbers[3], numbers[4]};
        UmiCreativeAudioClip *clip = NULL;
        UmiCreativeExport output = {0};
        UmiStatus status = UmiCreativeAudioLoadFile(argv[2], &clip);
        if (status == UMI_STATUS_OK) status = UmiCreativeAudioRender(clip, &edit, &output);
        if (status == UMI_STATUS_OK) status = UmiCreativeExportWriteNew(&output, argv[3]);
        UmiCreativeExportFree(&output); UmiCreativeAudioDestroy(clip); return Finish(status);
    }
    puts("Umicom native audio clip tools\n"
         "  --self-test\n  inspect ABSOLUTE_SOURCE.wav\n  demo ABSOLUTE_NEW.wav\n"
         "  trim ABSOLUTE_SOURCE.wav ABSOLUTE_NEW.wav BEGIN END GAIN_PERMILLE FADE_IN FADE_OUT\n"
         "All time arguments are sample frames; END is excluded. --help performs no I/O.");
    return argc == 2 && strcmp(argv[1], "--help") == 0 ? 0 : 2;
}

#ifdef _WIN32
/* Translate the Windows command line once at the platform boundary. File APIs
 * and the shared parser then receive UTF-8, independently of the active ANSI
 * code page. No shell command is reconstructed from these arguments. */
int wmain(int argc, wchar_t **wide)
{
    if (argc < 1 || argc > 16) return 2;
    char **argv = calloc((size_t)argc + 1U, sizeof(*argv));
    if (argv == NULL) return 1;
    int result = 1;
    for (int i = 0; i < argc; ++i) {
        int n = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[i], -1, NULL, 0, NULL, NULL);
        if (n < 1 || n > 16384) goto done;
        argv[i] = malloc((size_t)n);
        if (argv[i] == NULL || WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[i], -1, argv[i], n, NULL, NULL) != n) goto done;
    }
    result = CommandMain(argc, argv);
done:
    for (int i = 0; i < argc; ++i) free(argv[i]);
    free(argv); return result;
}
#else
int main(int argc, char **argv) { return CommandMain(argc, argv); }
#endif
