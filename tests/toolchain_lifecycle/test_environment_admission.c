/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_environment_admission.c
 *
 * PURPOSE:
 *   Exercise environment refusal, alias-safe updates and all-or-nothing composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/environment.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned allocations, writes;
static int refuse_allocation;
static UmiStatus write_status = UMI_STATUS_OK;
static char written[32768];
static void *StageAllocate(size_t count, size_t size)
{
    ++allocations;
    return refuse_allocation ? NULL : calloc(count, size);
}
static UmiStatus ObserveExport(const char *destination, const char *text)
{
    if (!destination || strcmp(destination, "synthetic export")) return UMI_STATUS_INVALID_ARGUMENT;
    ++writes;
    if (strlen(text) >= sizeof written) return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(written, text);
    return write_status;
}
/* Only allocation and final file export are substituted. No child process or
 * filesystem write occurs; the real environment policy and composition run. */
#define calloc StageAllocate
#define umi_fs_write_text ObserveExport
#include "../../src/toolchain/environment.c"
#undef umi_fs_write_text
#undef calloc

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); failed = 1; goto done; \
} } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int failed = 0;
    UmiEnvironmentPlan *base = calloc(1U, sizeof *base);
    UmiEnvironmentPlan *overlay = calloc(1U, sizeof *overlay);
    UmiEnvironmentPlan *output = calloc(1U, sizeof *output);
    UmiEnvironmentPlan *before = calloc(1U, sizeof *before);
    char *large = malloc(UMI_ENVIRONMENT_VALUE_CAPACITY + 1U);
    if (!base || !overlay || !output || !before || !large) { failed = 1; goto done; }
    CHECK(umi_environment_plan_add(base, "MODE", "base") == UMI_STATUS_OK);
    CHECK(umi_environment_plan_add(base, "KEEP", "yes") == UMI_STATUS_OK);
    CHECK(umi_environment_plan_add(overlay, "MODE", "overlay") == UMI_STATUS_OK);
    CHECK(umi_environment_plan_add(overlay, "NEW", "value") == UMI_STATUS_OK);
    CHECK(umi_environment_plan_add(output, "PREVIOUS", "retained") == UMI_STATUS_OK);
    *before = *output;
    const char *mode = argv[1];
    if (!strcmp(mode, "compose") || !strcmp(mode, "alias-base") ||
        !strcmp(mode, "alias-overlay") || !strcmp(mode, "alias-both")) {
        UmiEnvironmentPlan *destination = output;
        const UmiEnvironmentPlan *second = overlay;
        if (!strcmp(mode, "alias-base")) destination = base;
        else if (!strcmp(mode, "alias-overlay")) destination = overlay;
        else if (!strcmp(mode, "alias-both")) { destination = base; second = base; }
        CHECK(umi_environment_plan_compose(base, second, destination) == UMI_STATUS_OK);
        CHECK(destination->count == (!strcmp(mode, "alias-both") ? 2U : 3U));
        CHECK(!strcmp(umi_environment_plan_find(destination, "MODE"),
            !strcmp(mode, "alias-both") ? "base" : "overlay"));
        for (size_t i = 0; i < destination->count; ++i) {
            CHECK(destination->process_entries[i].name == destination->entries[i].name);
            CHECK(destination->process_entries[i].value == destination->entries[i].value);
        }
        if (destination != base) CHECK(!strcmp(umi_environment_plan_find(base, "MODE"), "base"));
        CHECK(allocations == 1U);
    } else if (!strcmp(mode, "empty-compose")) {
        CHECK(umi_environment_plan_compose(NULL, NULL, output) == UMI_STATUS_OK);
        CHECK(output->count == 0U);
    } else if (!strcmp(mode, "invalid-base") || !strcmp(mode, "invalid-overlay") ||
        !strcmp(mode, "allocation")) {
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        if (!strcmp(mode, "invalid-base")) memset(base->entries[0].name, 'x', sizeof base->entries[0].name);
        else if (!strcmp(mode, "invalid-overlay")) memset(overlay->entries[1].value, 'x', sizeof overlay->entries[1].value);
        else { refuse_allocation = 1; expected = UMI_STATUS_OUT_OF_MEMORY; }
        CHECK(umi_environment_plan_compose(base, overlay, output) == expected);
        CHECK(!memcmp(output, before, sizeof *output));
        CHECK(allocations == (refuse_allocation ? 1U : 0U));
    } else if (!strcmp(mode, "compose-capacity") || !strcmp(mode, "alias-capacity")) {
        umi_environment_plan_init(base);
        for (size_t i = 0; i < UMI_ENVIRONMENT_PLAN_MAX; ++i) {
            char key[32]; snprintf(key, sizeof key, "ITEM_%zu", i);
            CHECK(umi_environment_plan_add(base, key, "base") == UMI_STATUS_OK);
        }
        umi_environment_plan_init(overlay);
        CHECK(umi_environment_plan_add(overlay, "ITEM_0", "changed candidate") == UMI_STATUS_OK);
        CHECK(umi_environment_plan_add(overlay, "EXCESS", "value") == UMI_STATUS_OK);
        UmiEnvironmentPlan *destination = !strcmp(mode, "alias-capacity") ? base : output;
        *before = *destination;
        CHECK(umi_environment_plan_compose(base, overlay, destination) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!memcmp(destination, before, sizeof *destination));
        CHECK(!strcmp(umi_environment_plan_find(base, "ITEM_0"), "base"));
    } else if (!strcmp(mode, "set-self") || !strcmp(mode, "set-slice")) {
        const char *value = base->entries[0].value + (!strcmp(mode, "set-slice") ? 1 : 0);
        CHECK(umi_environment_plan_set(base, base->entries[0].name, value) == UMI_STATUS_OK);
        CHECK(!strcmp(umi_environment_plan_find(base, "MODE"),
            !strcmp(mode, "set-slice") ? "ase" : "base"));
    } else if (!strcmp(mode, "add-duplicate")) {
        *before = *base;
        CHECK(umi_environment_plan_add(base, "MODE", "new") == UMI_STATUS_ALREADY_EXISTS);
        CHECK(!memcmp(base, before, sizeof *base));
    } else if (!strcmp(mode, "invalid-name") || !strcmp(mode, "invalid-value") ||
        !strcmp(mode, "long-value")) {
        *before = *base;
        memset(large, 'x', UMI_ENVIRONMENT_VALUE_CAPACITY); large[UMI_ENVIRONMENT_VALUE_CAPACITY] = 0;
        const char *name = !strcmp(mode, "invalid-name") ? "BAD=NAME" : "MODE";
        const char *value = !strcmp(mode, "invalid-value") ? "line\nbreak" :
            !strcmp(mode, "long-value") ? large : "value";
        UmiStatus expected = !strcmp(mode, "long-value") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_ARGUMENT;
        CHECK(umi_environment_plan_set(base, name, value) == expected);
        CHECK(!memcmp(base, before, sizeof *base));
        CHECK(umi_environment_plan_add(base, name, value) == expected);
        CHECK(!memcmp(base, before, sizeof *base));
    } else if (!strcmp(mode, "count-read") || !strcmp(mode, "count-mutate") ||
        !strcmp(mode, "unterminated-read")) {
        if (!strcmp(mode, "unterminated-read")) memset(base->entries[1].name, 'x', sizeof base->entries[1].name);
        else base->count = UMI_ENVIRONMENT_PLAN_MAX + 1U;
        *before = *base;
        CHECK(umi_environment_plan_find(base, "MODE") == NULL);
        CHECK(umi_environment_plan_variables(base) == NULL);
        if (!strcmp(mode, "count-mutate")) {
            CHECK(umi_environment_plan_set(base, "MODE", "new") == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(umi_environment_plan_add(base, "OTHER", "new") == UMI_STATUS_CAPACITY_EXCEEDED);
        }
        CHECK(!memcmp(base, before, sizeof *base));
    } else if (!strcmp(mode, "export") || !strcmp(mode, "export-failure")) {
        if (!strcmp(mode, "export-failure")) write_status = UMI_STATUS_IO_ERROR;
        CHECK(umi_environment_plan_write(base, "synthetic export") == write_status);
        CHECK(writes == 1U && !strcmp(written, "MODE=base\nKEEP=yes\n"));
    } else if (!strcmp(mode, "export-invalid") || !strcmp(mode, "export-capacity")) {
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        if (!strcmp(mode, "export-invalid")) memset(base->entries[0].value, 'x', sizeof base->entries[0].value);
        else {
            umi_environment_plan_init(base);
            memset(large, 'x', UMI_ENVIRONMENT_VALUE_CAPACITY - 1U);
            large[UMI_ENVIRONMENT_VALUE_CAPACITY - 1U] = 0;
            for (size_t i = 0; i < 4U; ++i) {
                char key[16]; snprintf(key, sizeof key, "ITEM_%zu", i);
                CHECK(umi_environment_plan_add(base, key, large) == UMI_STATUS_OK);
            }
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        CHECK(umi_environment_plan_write(base, "synthetic export") == expected);
        CHECK(writes == 0U);
    } else { failed = 2; }
done:
    free(large); free(before); free(output); free(overlay); free(base);
    return failed;
}
