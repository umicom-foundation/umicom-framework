/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/source_naming.c
 *
 * PURPOSE:
 *   Detect version- and batch-labelled filenames before parallel copies can
 *   become competing sources of truth.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/source_naming.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Filename labels use these separators, while directory separators are only
 * used to locate the final path component. */
static int umi_codeguard_source_name_is_label_separator(char value)
{
    return value == '_' || value == '-' || value == '.';
}

/* A label may begin at the start of a filename or immediately after a common
 * filename separator. */
static int umi_codeguard_source_name_is_boundary(const char *name,
                                                 size_t index)
{
    return index == 0U ||
           umi_codeguard_source_name_is_label_separator(name[index - 1U]);
}

/* Compare ASCII text without depending on the process locale. */
static int umi_codeguard_source_name_has_prefix(const char *text,
                                                const char *prefix)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*prefix != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*text == '\0' ||
            tolower((unsigned char)*text) !=
                tolower((unsigned char)*prefix)) {
            return 0;
        }
        ++text;
        ++prefix;
    }
    return 1;
}

/* Consume one or more decimal digits and return the first following index. */
static size_t umi_codeguard_source_name_consume_digits(const char *name,
                                                       size_t index)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (isdigit((unsigned char)name[index]) != 0) {
        ++index;
    }
    return index;
}

/* A complete label must end before another label, the extension, or the end
 * of the filename. This avoids treating words such as "riscv64" as versions. */
static int umi_codeguard_source_name_is_label_end(const char *name,
                                                  size_t index)
{
    return name[index] == '\0' ||
           umi_codeguard_source_name_is_label_separator(name[index]);
}

/* Return only the final path component so directory names do not affect the
 * policy decision for a file. */
static const char *umi_codeguard_source_name_basename(const char *path)
{
    const char *name = path;
    const char *cursor;

    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = path; *cursor != '\0'; ++cursor) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '/' || *cursor == '\\') {
            name = cursor + 1;
        }
    }
    return name;
}

/* Detect compact labels such as v2, version_3, batch24, and mb60. */
static UmiCodeGuardSourceNameIssue
umi_codeguard_source_name_classify_word_label(const char *name, size_t index)
{
    size_t digit_index;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!umi_codeguard_source_name_is_boundary(name, index)) {
        return UMI_CODEGUARD_SOURCE_NAME_OK;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (tolower((unsigned char)name[index]) == 'v' &&
        isdigit((unsigned char)name[index + 1U]) != 0) {
        digit_index = umi_codeguard_source_name_consume_digits(name,
                                                               index + 1U);
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_codeguard_source_name_is_label_end(name, digit_index)) {
            return UMI_CODEGUARD_SOURCE_NAME_VERSION_LABEL;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_codeguard_source_name_has_prefix(name + index, "version")) {
        digit_index = index + strlen("version");
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_codeguard_source_name_is_label_separator(name[digit_index])) {
            ++digit_index;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (isdigit((unsigned char)name[digit_index]) != 0) {
            digit_index = umi_codeguard_source_name_consume_digits(
                name, digit_index);
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_codeguard_source_name_is_label_end(name, digit_index)) {
                return UMI_CODEGUARD_SOURCE_NAME_VERSION_LABEL;
            }
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_codeguard_source_name_has_prefix(name + index, "batch")) {
        digit_index = index + strlen("batch");
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_codeguard_source_name_is_label_separator(name[digit_index])) {
            ++digit_index;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (isdigit((unsigned char)name[digit_index]) != 0) {
            digit_index = umi_codeguard_source_name_consume_digits(
                name, digit_index);
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_codeguard_source_name_is_label_end(name, digit_index)) {
                return UMI_CODEGUARD_SOURCE_NAME_BATCH_LABEL;
            }
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (tolower((unsigned char)name[index]) == 'm' &&
        tolower((unsigned char)name[index + 1U]) == 'b' &&
        isdigit((unsigned char)name[index + 2U]) != 0) {
        digit_index = umi_codeguard_source_name_consume_digits(name,
                                                               index + 2U);
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_codeguard_source_name_is_label_end(name, digit_index)) {
            return UMI_CODEGUARD_SOURCE_NAME_BATCH_LABEL;
        }
    }

    return UMI_CODEGUARD_SOURCE_NAME_OK;
}

/* Detect semantic-version labels such as 0.5.0 while leaving ordinary test
 * sequence numbers such as test_01.c untouched. */
static int umi_codeguard_source_name_has_semantic_version(const char *name,
                                                          size_t index)
{
    size_t cursor;
    unsigned separators = 0U;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!umi_codeguard_source_name_is_boundary(name, index) ||
        isdigit((unsigned char)name[index]) == 0) {
        return 0;
    }

    cursor = umi_codeguard_source_name_consume_digits(name, index);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (name[cursor] == '.' &&
           isdigit((unsigned char)name[cursor + 1U]) != 0 &&
           separators < 2U) {
        ++separators;
        cursor = umi_codeguard_source_name_consume_digits(name, cursor + 1U);
    }

    return separators >= 1U &&
           umi_codeguard_source_name_is_label_end(name, cursor);
}

/*
 * Provide the codeguard source name classify operation used by this module and its client
 * applications.
 */
UmiCodeGuardSourceNameIssue umi_codeguard_source_name_classify(
    const char *path)
{
    const char *name;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0') {
        return UMI_CODEGUARD_SOURCE_NAME_OK;
    }

    name = umi_codeguard_source_name_basename(path);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; name[index] != '\0'; ++index) {
        UmiCodeGuardSourceNameIssue issue =
            umi_codeguard_source_name_classify_word_label(name, index);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (issue != UMI_CODEGUARD_SOURCE_NAME_OK) {
            return issue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_codeguard_source_name_has_semantic_version(name, index)) {
            return UMI_CODEGUARD_SOURCE_NAME_VERSION_LABEL;
        }
    }

    return UMI_CODEGUARD_SOURCE_NAME_OK;
}

/*
 * Provide the codeguard source name issue text operation used by this module and its
 * client applications.
 */
const char *umi_codeguard_source_name_issue_text(
    UmiCodeGuardSourceNameIssue issue)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (issue) {
        case UMI_CODEGUARD_SOURCE_NAME_VERSION_LABEL:
            return "version label";
        case UMI_CODEGUARD_SOURCE_NAME_BATCH_LABEL:
            return "batch label";
        default:
            return "none";
    }
}

/*
 * Provide the codeguard source name audit operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_source_name_audit(const char *path,
                                          UmiCodeGuardResult *result)
{
    UmiCodeGuardSourceNameIssue issue;
    UmiCodeGuardFinding finding = {0};

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    issue = umi_codeguard_source_name_classify(path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (issue == UMI_CODEGUARD_SOURCE_NAME_OK) {
        return UMI_STATUS_OK;
    }

    (void)snprintf(finding.rule_id, sizeof(finding.rule_id), "%s",
                   issue == UMI_CODEGUARD_SOURCE_NAME_BATCH_LABEL
                       ? "CODEGUARD-NAME-BATCH-001"
                       : "CODEGUARD-NAME-VERSION-001");
    finding.severity = UMI_CODEGUARD_MEDIUM;
    finding.category = UMI_CODEGUARD_CATEGORY_QUALITY;
    finding.confidence = 100U;
    (void)snprintf(finding.path, sizeof(finding.path), "%s", path);
    finding.line = 1U;
    finding.column = 1U;
    (void)snprintf(
        finding.message, sizeof(finding.message),
        "Filename contains a %s and can create competing sources of truth.",
        umi_codeguard_source_name_issue_text(issue));
    (void)snprintf(
        finding.remediation, sizeof(finding.remediation),
        "Merge useful logic into one descriptively named canonical file, then "
        "use Git history for earlier versions.");
    return umi_codeguard_result_add(result, &finding);
}
