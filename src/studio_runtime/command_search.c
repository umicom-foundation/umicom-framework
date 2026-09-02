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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_search.h"

#include <ctype.h>
#include <string.h>

/* Provide the equal ci operation used by this module and its client applications. */
static int equal_ci(const char *left, const char *right)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*left != '\0' && *right != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (tolower((unsigned char)*left) !=
            tolower((unsigned char)*right)) {
            return 0;
        }
        ++left;
        ++right;
    }
    return *left == '\0' && *right == '\0';
}

/* Provide the starts ci operation used by this module and its client applications. */
static int starts_ci(const char *text, const char *query)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*query != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
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

/* Provide the contains ci operation used by this module and its client applications. */
static int contains_ci(const char *text, const char *query)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (query[0] == '\0') return 1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (; *text != '\0'; ++text) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (starts_ci(text, query)) return 1;
    }

    return 0;
}

/* Provide the score operation used by this module and its client applications. */
static uint32_t score(
    const char *command_id,
    const char *title,
    const char *category,
    const char *query)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (equal_ci(title, query) || equal_ci(command_id, query)) return 1000U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (starts_ci(title, query)) return 900U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (starts_ci(command_id, query)) return 850U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (contains_ci(title, query)) return 700U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_ci(command_id, query)) return 650U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (contains_ci(category, query)) return 500U;
    return 0U;
}

/* Provide the insert sorted operation used by this module and its client applications. */
static void insert_sorted(
    UmiStudioRuntimeCommandSearchResults *results,
    const UmiStudioRuntimeCommandSearchResult *item)
{
    size_t index;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (results->count < UMI_STUDIO_COMMAND_SEARCH_CAPACITY) {
        results->items[results->count++] = *item;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (item->score > results->items[results->count - 1U].score) {
        results->items[results->count - 1U] = *item;
        results->truncated = 1;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        results->truncated = 1;
        return;
    }

    index = results->count - 1U;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
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

/*
 * Provide the command already present operation used by this module and its client
 * applications.
 */
static int command_already_present(
    const UmiStudioRuntimeCommandSearchResults *results,
    const char *command_id)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < results->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(results->items[index].command_id, command_id) == 0) {
            return 1;
        }
    }

    return 0;
}

/*
 * Provide the studio command search operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_command_search(
    const UmiStudioRuntimeBindings *bindings,
    const char *query,
    UmiStudioRuntimeCommandSearchResults *out_results)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || bindings->commands == NULL ||
        bindings->shell_registry == NULL ||
        query == NULL || out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_results, 0, sizeof(*out_results));

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_command_registry_count(bindings->commands);
         ++index) {
        UmiCommandSnapshot command;
        UmiStudioRuntimeCommandSearchResult item;
        uint32_t match_score;

        /* Apply this branch only when its contract condition is satisfied. */
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
        /* Use the stable identifier comparison to choose the matching record or policy. */
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
     * does not. This makes menu-item search feel like a modern professional
     * search rather than a raw command-ID lookup.
     */
    for (index = 0U;
         index < umi_application_shell_registry_count(
             bindings->shell_registry);
         ++index) {
        UmiApplicationShellContribution contribution;
        UmiStudioRuntimeCommandSearchResult item;
        uint32_t match_score;

        /* Apply this branch only when its contract condition is satisfied. */
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
        /* Use the stable identifier comparison to choose the matching record or policy. */
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
