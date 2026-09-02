/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/command_search_provider.c
 *
 * PURPOSE:
 *   Search commands by ID, title and category while retaining command
 *   enabled/disabled state in the result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/command_search_provider.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/* Provide the search commands operation used by this module and its client applications. */
static UmiStatus search_commands(
    void *user_data,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count)
{
    UmiCommandRegistry *commands = (UmiCommandRegistry *)user_data;
    size_t index;
    size_t used = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (commands == NULL || query == NULL ||
        out_results == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_command_registry_count(commands) && used < capacity;
         ++index) {
        UmiCommandSnapshot command;
        int32_t score;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_command_registry_at(
                commands, index, &command) != UMI_STATUS_OK) {
            continue;
        }

        score = umi_developer_workbench_search_score(
            query,
            command.title,
            command.command_id);

        /* Apply this branch only when its contract condition is satisfied. */
        if (score < 0) {
            score = umi_developer_workbench_search_score(
                query,
                command.category,
                command.description);
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (score < 0) continue;

        (void)memset(&out_results[used], 0, sizeof(out_results[used]));
        out_results[used].kind = UMI_DEVELOPER_WORKBENCH_SEARCH_COMMAND;
        copy_text(out_results[used].provider_id,
                  sizeof(out_results[used].provider_id),
                  "developer.search.commands");
        copy_text(out_results[used].result_id,
                  sizeof(out_results[used].result_id),
                  command.command_id);
        copy_text(out_results[used].title,
                  sizeof(out_results[used].title),
                  command.title);
        (void)snprintf(out_results[used].detail,
                       sizeof(out_results[used].detail),
                       "%s — %s",
                       command.category,
                       command.description);
        copy_text(out_results[used].command_id,
                  sizeof(out_results[used].command_id),
                  command.command_id);
        out_results[used].enabled =
            umi_command_registry_is_enabled(
                commands, command.command_id, NULL);
        out_results[used].score = score;
        out_results[used].revision = 1U;
        ++used;
    }

    *out_count = used;
    return used > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/*
 * Initialise developer workbench command search provider from caller-provided values so
 * later operations receive a known state.
 */
void umi_developer_workbench_command_search_provider_init(
    UmiDeveloperWorkbenchSearchProvider *provider,
    UmiCommandRegistry *commands)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL) return;

    (void)memset(provider, 0, sizeof(*provider));
    copy_text(provider->provider_id,
              sizeof(provider->provider_id),
              "developer.search.commands");
    copy_text(provider->title,
              sizeof(provider->title),
              "Commands");
    provider->kind = UMI_DEVELOPER_WORKBENCH_SEARCH_COMMAND;
    provider->priority = 100;
    provider->user_data = commands;
    provider->search = search_commands;
}
