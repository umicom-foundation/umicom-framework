/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_catalogue_integrity.c
 *
 * PURPOSE:
 *   Verify catalogue structural bounds, rejected append preservation and borrowed target inputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/sdk/export_catalogue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); result = 1; goto cleanup; \
} } while (0)

int main(int argc, char **argv)
{
    int result = 0;
    UmiSdkExportCatalogue *catalogue = malloc(sizeof *catalogue);
    unsigned char *before = malloc(sizeof *catalogue);
    CHECK(catalogue != NULL && before != NULL && argc == 2);
    const char *mode = argv[1];
    umi_sdk_export_catalogue_init(catalogue);
    CHECK(umi_sdk_export_catalogue_add(catalogue, "base", "Umicom::base", 1) == UMI_STATUS_OK);
    CHECK(umi_sdk_export_catalogue_add(catalogue, "sdk", "Umicom::sdk", 1) == UMI_STATUS_OK);
    if (strcmp(mode, "null-catalogue") == 0) {
        CHECK(umi_sdk_export_catalogue_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_sdk_export_catalogue_find(NULL, "base") == NULL);
        CHECK(!umi_sdk_export_catalogue_ready(NULL));
        CHECK(umi_sdk_export_catalogue_add(NULL, "x", "X", 1) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(mode, "empty") == 0) {
        umi_sdk_export_catalogue_init(catalogue);
        CHECK(umi_sdk_export_catalogue_validate(catalogue) == UMI_STATUS_OK);
        CHECK(!umi_sdk_export_catalogue_ready(catalogue));
        CHECK(umi_sdk_export_catalogue_find(catalogue, "base") == NULL);
    } else if (strcmp(mode, "corrupt-count") == 0 || strcmp(mode, "max-count") == 0 ||
               strncmp(mode, "corrupt-", 8U) == 0) {
        if (strcmp(mode, "corrupt-count") == 0) catalogue->count = UMI_SDK_EXPORT_CAPACITY + 1U;
        else if (strcmp(mode, "max-count") == 0) catalogue->count = SIZE_MAX;
        else if (strcmp(mode, "corrupt-id") == 0)
            memset(catalogue->targets[1].component_id, 'x', sizeof catalogue->targets[1].component_id);
        else if (strcmp(mode, "corrupt-target") == 0)
            memset(catalogue->targets[1].target_name, 'x', sizeof catalogue->targets[1].target_name);
        else if (strcmp(mode, "corrupt-empty-id") == 0) catalogue->targets[1].component_id[0] = '\0';
        else { CHECK(strcmp(mode, "corrupt-empty-target") == 0); catalogue->targets[1].target_name[0] = '\0'; }
        memcpy(before, catalogue, sizeof *catalogue);
        CHECK(umi_sdk_export_catalogue_validate(catalogue) == UMI_STATUS_INVALID_STATE);
        /* An earlier matching entry must not hide a malformed later entry. */
        CHECK(umi_sdk_export_catalogue_find(catalogue, "base") == NULL);
        CHECK(!umi_sdk_export_catalogue_ready(catalogue));
        CHECK(umi_sdk_export_catalogue_add(catalogue, "new", "New", 1) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(before, catalogue, sizeof *catalogue) == 0);
    } else if (strcmp(mode, "full") == 0) {
        for (size_t index = catalogue->count; index < UMI_SDK_EXPORT_CAPACITY; ++index) {
            char id[32], target[32];
            CHECK(snprintf(id, sizeof id, "component%zu", index) > 0);
            CHECK(snprintf(target, sizeof target, "Target%zu", index) > 0);
            CHECK(umi_sdk_export_catalogue_add(catalogue, id, target, 0) == UMI_STATUS_OK);
        }
        memcpy(before, catalogue, sizeof *catalogue);
        CHECK(umi_sdk_export_catalogue_ready(catalogue));
        CHECK(umi_sdk_export_catalogue_add(catalogue, "extra", "Extra", 0) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, catalogue, sizeof *catalogue) == 0);
    } else if (strcmp(mode, "borrow-unused") == 0) {
        UmiSdkExportedTarget *slot = &catalogue->targets[catalogue->count];
        strcpy(slot->component_id, "new"); strcpy(slot->target_name, "Umicom::new");
        CHECK(umi_sdk_export_catalogue_add(catalogue, slot->component_id, slot->target_name, -2) == UMI_STATUS_OK);
        CHECK(catalogue->count == 3U && strcmp(slot->component_id, "new") == 0);
        CHECK(strcmp(slot->target_name, "Umicom::new") == 0 && slot->required == 1);
    } else if (strcmp(mode, "borrow-live") == 0) {
        CHECK(umi_sdk_export_catalogue_add(catalogue, "base-alias", catalogue->targets[0].target_name, 0) == UMI_STATUS_OK);
        CHECK(strcmp(catalogue->targets[2].target_name, "Umicom::base") == 0);
        CHECK(catalogue->targets[2].required == 0);
    } else if (strcmp(mode, "unused-garbage") == 0) {
        memset(&catalogue->targets[catalogue->count], 0xa5, sizeof catalogue->targets[0]);
        CHECK(umi_sdk_export_catalogue_validate(catalogue) == UMI_STATUS_OK);
        CHECK(umi_sdk_export_catalogue_find(catalogue, "sdk") == &catalogue->targets[1]);
        CHECK(umi_sdk_export_catalogue_add(catalogue, "new", "New", 1) == UMI_STATUS_OK);
        CHECK(strcmp(catalogue->targets[2].component_id, "new") == 0);
    } else if (strcmp(mode, "find-boundaries") == 0) {
        CHECK(umi_sdk_export_catalogue_find(catalogue, NULL) == NULL);
        CHECK(umi_sdk_export_catalogue_find(catalogue, "") == NULL);
        CHECK(umi_sdk_export_catalogue_find(catalogue, "missing") == NULL);
        CHECK(umi_sdk_export_catalogue_find(catalogue, "BASE") == NULL);
        CHECK(umi_sdk_export_catalogue_find(catalogue, "base") == &catalogue->targets[0]);
    } else {
        const char *id = "new", *target = "Umicom::new";
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        char oversized[UMI_SDK_EXPORT_ID_CAPACITY + 1U];
        memset(oversized, 'x', sizeof oversized - 1U); oversized[sizeof oversized - 1U] = '\0';
        if (strcmp(mode, "null-id") == 0) id = NULL;
        else if (strcmp(mode, "null-target") == 0) target = NULL;
        else if (strcmp(mode, "empty-id") == 0) id = "";
        else if (strcmp(mode, "empty-target") == 0) target = "";
        else if (strcmp(mode, "overflow-id") == 0) { id = oversized; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (strcmp(mode, "overflow-target") == 0) { target = oversized; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else { CHECK(strcmp(mode, "duplicate") == 0); id = "base"; expected = UMI_STATUS_ALREADY_EXISTS; }
        /* Compare unused-slot bytes too, so a failed append cannot silently
         * overwrite an owner's prepared record while leaving count unchanged. */
        memset(&catalogue->targets[catalogue->count], 0xa5, sizeof catalogue->targets[0]);
        memcpy(before, catalogue, sizeof *catalogue);
        CHECK(umi_sdk_export_catalogue_add(catalogue, id, target, 1) == expected);
        CHECK(memcmp(before, catalogue, sizeof *catalogue) == 0);
    }
cleanup:
    free(before);
    free(catalogue);
    return result;
}
