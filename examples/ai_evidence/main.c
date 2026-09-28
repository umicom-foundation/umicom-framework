/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native entry point. Network use requires an explicit local-model command. */
#include "lesson.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    bool verbose = false, local = false; uint16_t port = 0U; const char *model = NULL;
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        puts("umicom-ai-evidence --self-test | demo\n"
            "umicom-ai-evidence local --port <1024..65535> --model <served-id> --acknowledge-local\n"
            "local sends the built-in Notes practice prompt to 127.0.0.1 only. It does not start or download a model.");
        return 0;
    }
    if (argc == 2 && (strcmp(argv[1], "--self-test") == 0 || strcmp(argv[1], "demo") == 0)) {
        verbose = strcmp(argv[1], "demo") == 0;
    } else if (argc == 7 && strcmp(argv[1], "local") == 0 && strcmp(argv[2], "--port") == 0 &&
               strcmp(argv[4], "--model") == 0 && strcmp(argv[6], "--acknowledge-local") == 0) {
        char *end = NULL; errno = 0; unsigned long value = strtoul(argv[3], &end, 10);
        if (errno != 0 || end == argv[3] || *end != '\0' || argv[3][0] < '0' || argv[3][0] > '9' ||
            value < 1024UL || value > 65535UL || argv[5][0] == '\0') {
            fputs("Invalid port or model ID. No request was made.\n", stderr); return 2;
        }
        port = (uint16_t)value; model = argv[5]; local = true; verbose = true;
    } else { fputs("Use --help. No request was made.\n", stderr); return 2; }
    UmiStatus status = UmiAiEvidenceLesson(stdout, verbose, local, port, model);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "AI evidence lesson failed (status %d). No automatic retry.\n", (int)status);
        return status == UMI_STATUS_UNAVAILABLE ? 77 : 1;
    }
    return 0;
}
