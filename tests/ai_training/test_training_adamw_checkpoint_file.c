/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_adamw_checkpoint_file.c
 * PURPOSE: Validate Framework-owned exclusive file persistence without
 *          silently replacing a successful checkpoint generation.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_adamw_checkpoint_file.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/optimizer_checkpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <process.h>
#define GetProcessIdentifier() ((unsigned long)_getpid())
#else
#include <unistd.h>
#define GetProcessIdentifier() ((unsigned long)getpid())
#endif

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expr); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig cfg = {12U, 3U, 4U, 8U, 2U, 88U};
    UmiAiTrainingDecoder *model = NULL, *restored = NULL;
    UmiAiTrainingAdamW *opt = NULL, *saved = NULL;
    UmiAiTrainingAdamWConfig options;
    char path[180];
    FILE *stream;
    CHECK(snprintf(path, sizeof(path), "umicom-adamw-proof-%lu.umi",
          GetProcessIdentifier()) > 0);
    CHECK(UmiAiTrainingAdamWDefaults(&options) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(model, &options, &opt) == UMI_STATUS_OK);
    /* The file must not exist. Never delete a previously existing path. */
    stream = fopen(path, "rb");
    CHECK(stream == NULL);
    CHECK(UmiAiTrainingAdamWCheckpointSaveNewFile(model, opt, path)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointSaveNewFile(model, opt, path)
          == UMI_STATUS_ALREADY_EXISTS);
    CHECK(UmiAiTrainingAdamWCheckpointLoadFile(path, &restored, &saved)
          == UMI_STATUS_OK);
    size_t neededA = 0U, neededB = 0U;
    CHECK(UmiAiTrainingAdamWCheckpointSize(model, opt, &neededA)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointSize(restored, saved, &neededB)
          == UMI_STATUS_OK && neededA == neededB);
    CHECK(remove(path) == 0);
    UmiAiTrainingAdamWDestroy(saved);
    UmiAiTrainingAdamWDestroy(opt);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    puts("AdamW exclusive file checkpoint, load and no-clobber: PASS");
    return 0;
}
