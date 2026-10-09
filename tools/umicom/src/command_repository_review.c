/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_repository_review.c
 * PURPOSE: Expose the shared read-only repository review through the native command.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/vcs/working_tree_review.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Read a strictly bounded decimal timeout; punctuation and signed values are not accepted. */
static int ReviewTimeout(const char *text, uint32_t *out)
{
    uint32_t value = 0U;
    if (text == NULL || text[0] == '\0')
        return 0;
    for (const char *cursor = text; *cursor != '\0'; ++cursor)
    {
        if (*cursor < '0' || *cursor > '9')
            return 0;
        unsigned digit = (unsigned)(*cursor - '0');
        if (value > (UINT32_MAX - digit) / 10U)
            return 0;
        value = value * 10U + digit;
    }
    if (value == 0U)
        return 0;
    *out = value;
    return 1;
}

/* This command is a presentation adapter: inspection and child ownership rules remain in Framework.
 */
int umi_cli_command_repository_review(int argc, char **argv)
{
    const char *root = ".";
    int root_set = 0, children = 0, timeout_set = 0;
    UmiVcsWorkingTreeRequest request = {0};
    UmiVcsWorkingTree *tree = NULL;
    UmiVcsWorkingTreeSummary summary;
    char *text;
    UmiStatus status;
    if (argc < 0 || (argc != 0 && argv == NULL))
        return 2;
    if (argc == 1 && (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "-h") == 0))
    {
        puts("Usage: umicom repo review [PATH] [--children] [--timeout-ms MILLISECONDS]\n"
             "       umicom repo review --root PATH [--children]\n"
             "Read local status and explain child repository changes. No fetch, stage, commit or "
             "push.");
        return 0;
    }
    for (int index = 0; index < argc; ++index)
    {
        if (strcmp(argv[index], "--children") == 0 && !children)
            children = 1;
        else if (strcmp(argv[index], "--root") == 0 && !root_set && index + 1 < argc)
        {
            root = argv[++index];
            root_set = 1;
        }
        else if (strcmp(argv[index], "--timeout-ms") == 0 && !timeout_set && index + 1 < argc)
        {
            if (!ReviewTimeout(argv[++index], &request.timeout_ms))
            {
                fputs("The timeout must be a positive decimal millisecond count.\n", stderr);
                return 2;
            }
            timeout_set = 1;
        }
        else if (argv[index][0] != '-' && !root_set)
        {
            root = argv[index];
            root_set = 1;
        }
        else
        {
            fputs("Invalid review options. Use umicom repo review --help.\n", stderr);
            return 2;
        }
    }
    if (root[0] == '\0')
    {
        fputs("The repository path cannot be empty.\n", stderr);
        return 2;
    }
    request.repository_root = root;
    status = UmiVcsWorkingTreeRead(&request, &tree);
    if (status != UMI_STATUS_OK)
    {
        fprintf(stderr, "Repository inspection failed: %s\n", umi_status_text(status));
        return 1;
    }
    text = malloc(UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY);
    if (text == NULL)
    {
        UmiVcsWorkingTreeDestroy(tree);
        return 1;
    }
    status = UmiVcsWorkingTreeDescribe(tree, &summary);
    if (status == UMI_STATUS_OK)
        status = UmiVcsWorkingTreeSummaryText(tree, text, UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY);
    if (status == UMI_STATUS_OK && puts(text) == EOF)
        status = UMI_STATUS_IO_ERROR;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < summary.entries; ++index)
    {
        const UmiVcsWorkingTreeEntry *entry = UmiVcsWorkingTreeEntryAt(tree, index);
        if (children && !entry->submodule)
            continue;
        status =
            UmiVcsWorkingTreeEntryText(tree, index, text, UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY);
        if (status == UMI_STATUS_OK && (putchar('\n') == EOF || puts(text) == EOF))
            status = UMI_STATUS_IO_ERROR;
    }
    free(text);
    UmiVcsWorkingTreeDestroy(tree);
    if (status != UMI_STATUS_OK)
    {
        fprintf(stderr, "Repository review output failed: %s\n", umi_status_text(status));
        return 1;
    }
    return fflush(stdout) == 0 ? 0 : 1;
}
