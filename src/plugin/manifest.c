/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/manifest.c
 *
 * PURPOSE:
 *   Parse line-oriented plug-in manifests with explicit validation and bounded
 *   list fields.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/manifest.h"

#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Provide the trim operation used by this module and its client applications. */
static char *trim(char *text)
{
    char *end;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*text != '\0' && isspace((unsigned char)*text)) ++text;
    end = text + strlen(text);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (end > text && isspace((unsigned char)end[-1])) --end;
    *end = '\0'; return text;
}
/* Add list only after its inputs and available capacity have been checked. */
static UmiStatus list_add(char items[][UMI_PLUGIN_ITEM_CAPACITY], size_t *count, const char *value)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*count >= UMI_PLUGIN_LIST_MAX || strlen(value) >= UMI_PLUGIN_ITEM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(items[(*count)++], UMI_PLUGIN_ITEM_CAPACITY, "%s", value); return UMI_STATUS_OK;
}
/* Provide the parse version operation used by this module and its client applications. */
static UmiStatus parse_version(const char *value, UmiVersion *version)
{
    unsigned int major, minor, patch;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (sscanf(value, "%u.%u.%u", &major, &minor, &patch) != 3 || major > UINT16_MAX || minor > UINT16_MAX || patch > UINT16_MAX) return UMI_STATUS_PARSE_ERROR;
    version->major = (uint16_t)major; version->minor = (uint16_t)minor; version->patch = (uint16_t)patch; return UMI_STATUS_OK;
}
/*
 * Read plugin manifest into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_plugin_manifest_parse(const char *text, UmiPluginManifest *out_manifest)
{
    char *copy, *line, *next; UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_manifest, 0, sizeof(*out_manifest));
    copy = (char *)malloc(strlen(text) + 1U); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY; (void)strcpy(copy, text);
    line = copy;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (line != NULL && *line != '\0') {
        char *equals, *key, *value;
        next = strchr(line, '\n'); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (next != NULL) *next++ = '\0';
        key = trim(line);
        /* Apply this branch only when its contract condition is satisfied. */
        if (*key == '\0' || *key == '#') { line = next; continue; }
        equals = strchr(key, '='); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (equals == NULL) { status = UMI_STATUS_PARSE_ERROR; break; }
        *equals = '\0'; value = trim(equals + 1); key = trim(key);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(key, "id") == 0) (void)snprintf(out_manifest->plugin_id, sizeof(out_manifest->plugin_id), "%s", value);
        else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "name") == 0) (void)snprintf(out_manifest->display_name, sizeof(out_manifest->display_name), "%s", value);
        else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (strcmp(key, "version") == 0) status = parse_version(value, &out_manifest->version);
        else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (strcmp(key, "abi") == 0) { unsigned long abi = strtoul(value, NULL, 10); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (abi > UINT32_MAX) status = UMI_STATUS_PARSE_ERROR; /* Use this fallback path when the earlier condition does not apply. */ else out_manifest->required_abi = (uint32_t)abi; }
        else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "library") == 0) (void)snprintf(out_manifest->library_path, sizeof(out_manifest->library_path), "%s", value);
        else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (strcmp(key, "permission") == 0) status = list_add(out_manifest->permissions, &out_manifest->permission_count, value);
        else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (strcmp(key, "capability") == 0) status = list_add(out_manifest->capabilities, &out_manifest->capability_count, value);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        line = next;
    }
    free(copy); return status;
}
/*
 * Read plugin manifest into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_plugin_manifest_load(const char *path, UmiPluginManifest *out_manifest)
{
    FILE *stream; long size; char *text; size_t read; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    stream = fopen(path, "rb"); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (stream == NULL) return UMI_STATUS_IO_ERROR;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (fseek(stream, 0, SEEK_END) != 0 || (size = ftell(stream)) < 0 || fseek(stream, 0, SEEK_SET) != 0) { (void)fclose(stream); return UMI_STATUS_IO_ERROR; }
    text = (char *)malloc((size_t)size + 1U); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (text == NULL) { (void)fclose(stream); return UMI_STATUS_OUT_OF_MEMORY; }
    read = fread(text, 1U, (size_t)size, stream); (void)fclose(stream); text[read] = '\0'; status = read == (size_t)size ? umi_plugin_manifest_parse(text, out_manifest) : UMI_STATUS_IO_ERROR; free(text); return status;
}
/* Check that plugin manifest satisfies its contract before another service relies on it. */
UmiStatus umi_plugin_manifest_validate(const UmiPluginManifest *manifest, char *out_reason, size_t reason_capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || out_reason == NULL || reason_capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (manifest->plugin_id[0] == '\0') { (void)snprintf(out_reason, reason_capacity, "missing id"); return UMI_STATUS_INVALID_STATE; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (manifest->display_name[0] == '\0') { (void)snprintf(out_reason, reason_capacity, "missing name"); return UMI_STATUS_INVALID_STATE; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (manifest->required_abi == 0U) { (void)snprintf(out_reason, reason_capacity, "missing ABI"); return UMI_STATUS_INVALID_STATE; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (manifest->library_path[0] == '\0') { (void)snprintf(out_reason, reason_capacity, "missing library path"); return UMI_STATUS_INVALID_STATE; }
    (void)snprintf(out_reason, reason_capacity, "valid"); return UMI_STATUS_OK;
}
