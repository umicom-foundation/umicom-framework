/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/configure_definitions/test_edit.c
 * PURPOSE: Exercise literal configure edits, capacity refusal and preservation of other profile settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/build/configure_definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)
int main(int argc, char **argv)
{
    int failed = 0;
    UmiBuildProfile *profile = calloc(1U, sizeof *profile), *before = calloc(1U, sizeof *before);
    UmiArguments *parsed = calloc(1U, sizeof *parsed);
    CHECK(argc == 2 && profile != NULL && before != NULL && parsed != NULL);
    umi_build_profile_init(profile);
    strcpy(profile->compiler, "compiler-owned-by-profile");
    strcpy(profile->configure_definitions, "-DKEEP=one -DCHOICE:STRING=old -DAFTER=two");
    const char *mode = argv[1], *name = "CHOICE", *type = "PATH", *value = "C:/SDK with spaces";
    UmiStatus expected = UMI_STATUS_OK;
    int remove = 0;
    char longValue[UMI_BUILD_ARGUMENT_CAPACITY + 1U];
    memset(longValue, 'a', sizeof longValue - 1U); longValue[sizeof longValue - 1U] = '\0';
    if (strcmp(mode, "append") == 0) name = "NEW_NAME";
    else if (strcmp(mode, "empty_value") == 0) value = "";
    else if (strcmp(mode, "quoted_value") == 0) value = "a \"quoted\" value; literal $(text) \\ tail";
    else if (strcmp(mode, "untyped") == 0) type = NULL;
    else if (strcmp(mode, "remove") == 0) remove = 1;
    else if (strcmp(mode, "missing") == 0) { remove = 1; name = "ABSENT"; }
    else if (strcmp(mode, "invalid_name") == 0) { name = "BAD=VALUE"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "invalid_type") == 0) { type = "LIST"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "reserved") == 0) { name = "CMAKE_C_COMPILER"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "control") == 0) { value = "one\ntwo"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "duplicate") == 0) {
        strcpy(profile->configure_definitions, "-DX=1 -DX=2"); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "malformed") == 0) {
        strcpy(profile->configure_definitions, "-P foreign.cmake"); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "unterminated") == 0) {
        memset(profile->configure_definitions, 'x', sizeof profile->configure_definitions);
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "full") == 0 || strcmp(mode, "full_replace") == 0) {
        size_t used = 0U;
        for (size_t i = 0U; i < UMI_ARGUMENTS_CAPACITY; ++i) {
            int count = snprintf(profile->configure_definitions + used,
                sizeof profile->configure_definitions - used, "%s-DITEM%zu=x", i == 0U ? "" : " ", i);
            CHECK(count > 0 && (size_t)count < sizeof profile->configure_definitions - used);
            used += (size_t)count;
        }
        if (strcmp(mode, "full") == 0) { name = "NEW"; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else name = "ITEM8";
    } else if (strcmp(mode, "alias") == 0) value = profile->compiler;
    else if (strcmp(mode, "token_capacity") == 0) { value = longValue; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(mode, "text_capacity") == 0) {
        longValue[400] = '\0'; value = longValue; name = "THREE";
        CHECK(snprintf(profile->configure_definitions, sizeof profile->configure_definitions,
            "-DONE=%s -DTWO=%s", value, value) > 0);
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else CHECK(strcmp(mode, "replace") == 0 || strcmp(mode, "null") == 0);
    *before = *profile;
    if (strcmp(mode, "null") == 0) {
        CHECK(UmiBuildConfigureDefinitionSet(NULL, name, type, value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildConfigureDefinitionSet(profile, NULL, type, value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildConfigureDefinitionSet(profile, name, type, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildConfigureDefinitionRemove(profile, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(profile, before, sizeof *profile) == 0); goto cleanup;
    }
    UmiStatus status = remove ? UmiBuildConfigureDefinitionRemove(profile, name) :
        UmiBuildConfigureDefinitionSet(profile, name, type, value);
    CHECK(status == expected);
    if (status != UMI_STATUS_OK || strcmp(mode, "missing") == 0) {
        CHECK(memcmp(profile, before, sizeof *profile) == 0); goto cleanup;
    }
    CHECK(UmiBuildConfigureDefinitions(profile, parsed) == UMI_STATUS_OK);
    if (remove) {
        CHECK(parsed->count == 2U && strcmp(parsed->values[0], "-DKEEP=one") == 0 &&
            strcmp(parsed->values[1], "-DAFTER=two") == 0);
    } else {
        size_t index = strcmp(mode, "append") == 0 ? 3U : strcmp(mode, "full_replace") == 0 ? 8U : 1U;
        const char *equals = strchr(parsed->values[index], '=');
        CHECK(equals != NULL && strcmp(equals + 1, value) == 0);
        if (strcmp(mode, "full_replace") == 0) CHECK(parsed->count == UMI_ARGUMENTS_CAPACITY);
        else CHECK(strcmp(parsed->values[0], "-DKEEP=one") == 0 &&
            strcmp(parsed->values[2], "-DAFTER=two") == 0);
    }
    /* Restore only the edited field, then compare every other setting. */
    memcpy(profile->configure_definitions, before->configure_definitions, sizeof profile->configure_definitions);
    CHECK(memcmp(profile, before, sizeof *profile) == 0);
cleanup:
    free(parsed); free(before); free(profile);
    return failed;
}
