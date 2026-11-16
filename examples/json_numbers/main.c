/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/json_numbers/main.c
 *
 * PURPOSE:
 *   Read approximate generation settings and exact integer seeds with one installed native JSON owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/language_runtime/json_tree.h"
#include "umicom/base/ascii_number.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

typedef struct GenerationSettings
{
    double guidance;
    int64_t steps;
    int64_t seed;
} GenerationSettings;

/* This host policy is intentionally separate from generic JSON conversion.
 * Stage every field, reject ambiguous keys and publish only a complete,
 * acceptable settings value. No model is contacted or generation started. */
static UmiStatus ReadSettings(const char *json, GenerationSettings *out)
{
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(json, strlen(json), NULL, NULL, &tree);
    GenerationSettings staged = {0};
    int node = -1;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeValidateUniqueKeys(tree, 0, NULL);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, 0, "guidance", &node);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeDouble(tree, node, &staged.guidance);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, 0, "steps", &node);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, node, &staged.steps);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, 0, "seed", &node);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, node, &staged.seed);
    if (status == UMI_STATUS_OK &&
        (staged.guidance < 0.0 || staged.guidance > 30.0 ||
         staged.steps < 1 || staged.steps > 150 || staged.seed < 0))
        status = UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK)
        *out = staged;
    return status;
}

int main(void)
{
    GenerationSettings settings = {0};
    const char *accepted =
        "{\"guidance\":7.5,\"steps\":20,\"seed\":9007199254740993}";
    if (ReadSettings(accepted, &settings) != UMI_STATUS_OK ||
        settings.guidance != 7.5 || settings.steps != 20 ||
        settings.seed != INT64_C(9007199254740993))
        return 1;
    GenerationSettings before;
    memcpy(&before, &settings, sizeof(before));
    const char *refused[] = {
        "{\"guidance\":1e9999,\"steps\":20,\"seed\":7}",
        "{\"guidance\":31,\"steps\":20,\"seed\":7}",
        "{\"guidance\":7.5,\"steps\":20.0,\"seed\":7}",
        "{\"guidance\":7.5,\"steps\":20,\"seed\":-1}",
        "{\"guidance\":7.5,\"steps\":20,\"seed\":7,\"seed\":8}"
    };
    for (size_t i = 0U; i < sizeof(refused) / sizeof(refused[0]); ++i)
        if (ReadSettings(refused[i], &settings) == UMI_STATUS_OK ||
            memcmp(&before, &settings, sizeof(before)) != 0)
            return 1;
    char guidance[64];
    size_t length = 0U;
    if (umi_ascii_double_format(settings.guidance, guidance, sizeof(guidance), &length) != UMI_STATUS_OK)
        return 1;
    if (printf("guidance=%s steps=%" PRId64 " seed=%" PRId64 "\n",
        guidance, settings.steps, settings.seed) < 0 ||
        puts("Settings inspected locally; no provider or generation request.") == EOF)
        return 1;
    return fflush(stdout) == 0 ? 0 : 1;
}
