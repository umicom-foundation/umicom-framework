/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_search.c
 *
 * PURPOSE:
 *   Implement deterministic case-insensitive command/menu search.
 *
 * SCORING:
 *   Exact title/ID > title/ID prefix > title substring > ID/category substring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_search.h"

#include <ctype.h>
#include <string.h>

static int equal_ci(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        if (tolower((unsigned char)*left) !=
            tolower((unsigned char)*right)) {
            return 0;
        }
        ++left;
        ++right;
    }
    return *left == '\0' && *right == '\0';
}

static int starts_ci(const char *text, const char *query)
{
    while (*query != '\0') {
        if (*text == '\0' ||
            tolower((unsigned char)*text) !=
            tolower((unsigned char)*query)) {
            return 0;
        }
        ++text;
        ++query;
    }
    return 1;
}

static int contains_ci(const char *text, const char *query)
{
    if (query[0] == '\0') return 1;

    for (; *text != '\0'; ++text) {
        if (starts_ci(text, query)) return 1;
    }

    return 0;
}

static uint32_t score(
    const char *command_id,
    const char *title,
    const char *category,
    const char *query)
{
    if (equal_ci(title, query) || equal_ci(command_id, query)) return 1000U;
    if (starts_ci(title, query)) return 900U;
    if (starts_ci(command_id, query)) return 850U;
    if (contains_ci(title, query)) return 700U;
    if (contains_ci(command_id, query)) return 650U;
    if (contains_ci(category, query)) return 500U;
    return 0U;
}

static void insert_sorted(
    UmiStudioRuntimeCommandSearchResults *results,
    const UmiStudioRuntimeCommandSearchResult *item)
{
    size_t index;

    if (results->count < UMI_STUDIO_COMMAND_SEARCH_CAPACITY) {
        results->items[results->count++] = *item;
    } else if (item->score > results->items[results->count - 1U].score) {
        results->items[results->count - 1U] = *item;
        results->truncated = 1;
    } else {
        results->truncated = 1;
        return;
    }

    index = results->count - 1U;

    while (index > 0U &&
           results->items[index].score >
               results->items[index - 1U].score) {
        UmiStudioRuntimeCommandSearchResult temporary =
            results->items[index - 1U];
        results->items[index - 1U] = results->items[index];
        results->items[index] = temporary;
        index -= 1U;
    }
}

static int command_already_present(
    const UmiStudioRuntimeCommandSearchResults *results,
    const char *command_id)
{
    size_t index;

    for (index = 0U; index < results->count; ++index) {
        if (strcmp(results->items[index].command_id, command_id) == 0) {
            return 1;
        }
    }

    return 0;
}

UmiStatus umi_studio_command_search(
    const UmiStudioRuntimeBindings *bindings,
    const char *query,
    UmiStudioRuntimeCommandSearchResults *out_results)
{
    size_t index;

    if (bindings == NULL || bindings->commands == NULL ||
        bindings->shell_registry == NULL ||
        query == NULL || out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_results, 0, sizeof(*out_results));

    for (index = 0U;
         index < umi_command_registry_count(bindings->commands);
         ++index) {
        UmiCommandSnapshot command;
        UmiStudioRuntimeCommandSearchResult item;
        uint32_t match_score;

        if (umi_command_registry_at(
                bindings->commands,
                index,
                &command) != UMI_STATUS_OK) {
            continue;
        }

        match_score = score(
            command.command_id,
            command.title,
            command.category,
            query);
        if (match_score == 0U) continue;

        (void)memset(&item, 0, sizeof(item));
        (void)strcpy(item.command_id, command.command_id);
        (void)strcpy(item.title, command.title);
        (void)strcpy(item.category, command.category);
        item.score = match_score;
        item.enabled = umi_command_registry_is_enabled(
            bindings->commands,
            command.command_id,
            NULL);

        insert_sorted(out_results, &item);
    }

    /*
     * Include shell contributions whose labels match even if their command title
     * does not. This is what makes menu-item search feel like modern IDE/TWS
     * search rather than a raw command-ID lookup.
     */
    for (index = 0U;
         index < umi_application_shell_registry_count(
             bindings->shell_registry);
         ++index) {
        UmiApplicationShellContribution contribution;
        UmiStudioRuntimeCommandSearchResult item;
        uint32_t match_score;

        if (umi_application_shell_registry_at(
                bindings->shell_registry,
                index,
                &contribution) != UMI_STATUS_OK ||
            contribution.command_id[0] == '\0' ||
            command_already_present(
                out_results,
                contribution.command_id)) {
            continue;
        }

        match_score = score(
            contribution.command_id,
            contribution.title,
            contribution.group_id,
            query);
        if (match_score == 0U) continue;

        (void)memset(&item, 0, sizeof(item));
        (void)strcpy(item.command_id, contribution.command_id);
        (void)strcpy(item.title, contribution.title);
        (void)strcpy(item.category, contribution.group_id);
        (void)strcpy(
            item.contribution_id,
            contribution.contribution_id);
        item.score = match_score;
        item.enabled =
            (contribution.flags & UMI_APPLICATION_SHELL_ENABLED) != 0U;
        item.from_shell = 1;

        insert_sorted(out_results, &item);
    }

    return UMI_STATUS_OK;
}
