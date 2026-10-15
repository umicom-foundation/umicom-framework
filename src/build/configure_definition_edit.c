/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/configure_definition_edit.c
 * PURPOSE: Edit literal CMake definitions transactionally through the existing argument grammar.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/build/configure_definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Both public operations validate a candidate token through the normal parser.
 * This keeps reserved-name and type rules in one place as new settings appear. */
static UmiStatus DefinitionToken(const char *name, const char *type, const char *value,
    char *token, size_t capacity)
{
    if (name == NULL || name[0] == '\0' || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    int written = type != NULL && type[0] != '\0'
        ? snprintf(token, capacity, "-D%s:%s=%s", name, type, value)
        : snprintf(token, capacity, "-D%s=%s", name, value);
    if (written < 0 || (size_t)written >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A name may not smuggle type/value punctuation into the assembled token. */
    size_t length = strcspn(name, ":=");
    if (name[length] != '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (type != NULL && strpbrk(type, ":=") != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static UmiStatus EditDefinition(UmiBuildProfile *profile, const char *name,
    const char *type, const char *value, int remove)
{
    if (profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char token[UMI_BUILD_ARGUMENT_CAPACITY];
    UmiStatus status = DefinitionToken(name, type, value, token, sizeof token);
    if (status != UMI_STATUS_OK) return status;
    UmiBuildProfile *candidate = malloc(sizeof *candidate);
    UmiArguments *arguments = malloc(sizeof *arguments);
    UmiArguments *checked = malloc(sizeof *checked);
    if (candidate == NULL || arguments == NULL || checked == NULL) {
        free(candidate); free(arguments); free(checked);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *candidate = *profile;
    const char *single[] = {token};
    status = UmiArgumentsFormat(single, 1U, candidate->configure_definitions,
        sizeof candidate->configure_definitions);
    if (status == UMI_STATUS_OK) status = UmiBuildConfigureDefinitions(candidate, checked);
    if (status == UMI_STATUS_OK) status = UmiBuildConfigureDefinitions(profile, arguments);
    const char *values[UMI_ARGUMENTS_CAPACITY];
    size_t count = 0U;
    int found = 0;
    /* Only pointer projections change here. The parsed list and token own their
     * bytes until the completed encoding has been checked and copied back. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < arguments->count; ++i) {
        const char *existing = arguments->values[i] + 2U;
        size_t length = strcspn(existing, ":=");
        if (strlen(name) == length && memcmp(existing, name, length) == 0) {
            found = 1;
            if (!remove) values[count++] = token;
        } else values[count++] = arguments->values[i];
    }
    if (status == UMI_STATUS_OK && !remove && !found) {
        if (count == UMI_ARGUMENTS_CAPACITY) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else values[count++] = token;
    }
    if (status == UMI_STATUS_OK && !(remove && !found)) {
        status = UmiArgumentsFormat(values, count, candidate->configure_definitions,
            sizeof candidate->configure_definitions);
        if (status == UMI_STATUS_OK) status = UmiBuildConfigureDefinitions(candidate, checked);
        if (status == UMI_STATUS_OK) {
            /* Other profile settings are deliberately untouched: this operation
             * cannot select a compiler, grant trust, or start configuration. */
            memcpy(profile->configure_definitions, candidate->configure_definitions,
                sizeof profile->configure_definitions);
        }
    }
    free(checked); free(arguments); free(candidate);
    return status;
}

UmiStatus UmiBuildConfigureDefinitionSet(UmiBuildProfile *profile,
    const char *name, const char *type, const char *value)
{
    return EditDefinition(profile, name, type, value, 0);
}

UmiStatus UmiBuildConfigureDefinitionRemove(UmiBuildProfile *profile, const char *name)
{
    return EditDefinition(profile, name, NULL, "", 1);
}
