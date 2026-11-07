/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/identity_directory.c
 *
 * PURPOSE:
 *   Implement the identity directory behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/identity_directory.h"
#include <string.h>
/* The original directory operations trusted public counts and text arrays.
 * Bounded validation and checked revision publication below replace them; the
 * earlier implementation remains disabled for engineering review. */
#if 0
/*
 * Find security identity directory while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiSecurityIdentityProfile *umi_security_identity_directory_find(const UmiSecurityIdentityDirectory *directory,const char *identity_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (directory == NULL || identity_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < directory->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(directory->items[index].identity_id,identity_id) == 0) return &directory->items[index];
    return NULL;
}
/*
 * Add security identity directory only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_security_identity_directory_add(UmiSecurityIdentityDirectory *directory,const UmiSecurityIdentityProfile *identity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (directory == NULL || identity == NULL || identity->identity_id[0] == '\0' || identity->display_name[0] == '\0' || identity->kind < UMI_IDENTITY_HUMAN || identity->kind > UMI_IDENTITY_AGENT) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_security_identity_directory_find(directory,identity->identity_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (directory->count >= UMI_SECURITY_GOVERNANCE_MAX_IDENTITIES) return UMI_STATUS_CAPACITY_EXCEEDED;
    directory->items[directory->count] = *identity;
    directory->items[directory->count].revision = 1U;
    directory->count += 1U; directory->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the security identity directory set enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_security_identity_directory_set_enabled(UmiSecurityIdentityDirectory *directory,const char *identity_id,bool enabled)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (directory == NULL || identity_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < directory->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(directory->items[index].identity_id,identity_id) == 0) { directory->items[index].enabled = enabled; directory->items[index].revision += 1U; directory->revision += 1U; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
#endif

/* These public records may be filled by callers or loaded from storage. Check
 * the complete used region before lookup; an early match must not hide a
 * malformed later row. Applications share this boundary instead of copying it. */
static bool directory_text_valid(const char *text, size_t capacity, bool empty_allowed)
{
    if (text == NULL) return false;
    for (size_t i = 0U; i < capacity; ++i) {
        if (text[i] == '\0') return empty_allowed || i != 0U;
    }
    return false;
}

static bool directory_profile_valid(const UmiSecurityIdentityProfile *identity)
{
    return identity != NULL &&
        directory_text_valid(identity->identity_id, sizeof identity->identity_id, false) &&
        directory_text_valid(identity->display_name, sizeof identity->display_name, false) &&
        directory_text_valid(identity->organisation, sizeof identity->organisation, true) &&
        identity->kind >= UMI_IDENTITY_HUMAN && identity->kind <= UMI_IDENTITY_AGENT;
}

static bool directory_valid(const UmiSecurityIdentityDirectory *directory)
{
    if (directory == NULL || directory->count > UMI_SECURITY_GOVERNANCE_MAX_IDENTITIES)
        return false;
    for (size_t i = 0U; i < directory->count; ++i) {
        if (!directory_profile_valid(&directory->items[i])) return false;
    }
    return true;
}

static size_t directory_index(const UmiSecurityIdentityDirectory *directory,
                              const char *identity_id)
{
    for (size_t i = 0U; i < directory->count; ++i)
        if (strcmp(directory->items[i].identity_id, identity_id) == 0) return i;
    return SIZE_MAX;
}

const UmiSecurityIdentityProfile *umi_security_identity_directory_find(
    const UmiSecurityIdentityDirectory *directory, const char *identity_id)
{
    if (!directory_valid(directory) ||
        !directory_text_valid(identity_id, UMI_SECURITY_GOVERNANCE_ID_CAPACITY, false))
        return NULL;
    const size_t index = directory_index(directory, identity_id);
    return index == SIZE_MAX ? NULL : &directory->items[index];
}

UmiStatus umi_security_identity_directory_add(UmiSecurityIdentityDirectory *directory,
                                              const UmiSecurityIdentityProfile *identity)
{
    if (!directory_valid(directory) || !directory_profile_valid(identity))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (directory_index(directory, identity->identity_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    if (directory->count == UMI_SECURITY_GOVERNANCE_MAX_IDENTITIES ||
        directory->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Snapshot before publication also supports a candidate in an unused slot
     * of the caller's directory. No operation after this point can fail. */
    UmiSecurityIdentityProfile candidate = *identity;
    candidate.revision = 1U;
    directory->items[directory->count] = candidate;
    ++directory->count;
    ++directory->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_security_identity_directory_set_enabled(UmiSecurityIdentityDirectory *directory,
                                                      const char *identity_id, bool enabled)
{
    if (!directory_valid(directory) ||
        !directory_text_valid(identity_id, UMI_SECURITY_GOVERNANCE_ID_CAPACITY, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    const size_t index = directory_index(directory, identity_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Both revisions describe the same edit. Refuse exhaustion before changing
     * either counter or enabled state, including repeated assignments. */
    if (directory->revision == UINT64_MAX || directory->items[index].revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    directory->items[index].enabled = enabled;
    ++directory->items[index].revision;
    ++directory->revision;
    return UMI_STATUS_OK;
}
