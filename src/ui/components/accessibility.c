/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/accessibility.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/accessibility.h"

#include <string.h>
#include "umicom/base/text.h"

/* Provide the copy text operation used by this module and its client applications. */
/* Use Framework bounded replacement for staged metadata and preserve the earlier unbounded helper for review. The previous implementation is retained for review. */
#if 0
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    const size_t length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
#endif
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    return umi_text_replace(destination, capacity, source);
}

/*
 * Copy ui component accessibility into module-owned storage so callers keep ownership of
 * their input values.
 */
/* Publish accessibility name and description together after both fit. The previous sequential mutation is retained to explain the ownership correction. The previous implementation is retained for review. */
#if 0
UmiStatus umi_ui_component_accessibility_set(UmiUiComponentAccessibility *value,
                                             const char *name,
                                             const char *description)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = copy_text(value->name,
                                 sizeof(value->name),
                                 name != NULL ? name : "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return copy_text(value->description,
                     sizeof(value->description),
                     description != NULL ? description : "");
}
#endif
UmiStatus umi_ui_component_accessibility_set(UmiUiComponentAccessibility *value,
                                             const char *name,
                                             const char *description)
{
    UmiUiComponentAccessibility staged = {0};
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = copy_text(staged.name, sizeof(staged.name),
                                 name != NULL ? name : "");
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(staged.description, sizeof(staged.description),
                       description != NULL ? description : "");
    if (status != UMI_STATUS_OK) return status;
    /* Both inputs can borrow either destination field; commit only after both
     * have been captured. No failed edit leaves a half-updated accessible label. */
    *value = staged;
    return UMI_STATUS_OK;
}
