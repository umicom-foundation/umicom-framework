/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_navigation.c
 *
 * PURPOSE:
 *   Keep offline resource navigation inside the chosen manifest and root.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_pack_internal.h"
#include "umicom/teacher/learning_navigation.h"
#include "umicom/platform/rooted_files.h"
#include <string.h>

/* Metadata can retain Windows slash spelling, but two names which refer to the
 * same portable path must not choose different declarations by array order. */
static void NavigationSlashes(char *text)
{
    for (size_t i = 0U; text[i] != '\0'; ++i)
        if (text[i] == '\\') text[i] = '/';
}
UmiStatus UmiLearningPackFindPath(const UmiLearningPack *pack, const char *relative,
    UmiLearningPackResource *out)
{
    if (pack == NULL || relative == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_TEACHER_PATH_CAPACITY && relative[length] != '\0') ++length;
    if (length == UMI_TEACHER_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiRootedFileValidatePath(pack->root, relative);
    if (status != UMI_STATUS_OK) return status;
    char wanted[UMI_TEACHER_PATH_CAPACITY];
    memcpy(wanted, relative, length + 1U);
    NavigationSlashes(wanted);
    const UmiLearningPackResource *found = NULL;
    for (size_t i = 0U; i < pack->info.resource_count; ++i) {
        char candidate[UMI_TEACHER_PATH_CAPACITY];
        memcpy(candidate, pack->resources[i].path, sizeof(candidate));
        NavigationSlashes(candidate);
        if (strcmp(candidate, wanted) == 0) {
            if (found != NULL) return UMI_STATUS_ALREADY_EXISTS;
            found = &pack->resources[i];
        }
    }
    if (found == NULL) return UMI_STATUS_NOT_FOUND;
    *out = *found;
    return UMI_STATUS_OK;
}
static int NavigationAnchorCharacter(unsigned char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || ch == '_' || ch == '-' || ch == '.' || ch == ':';
}
UmiStatus UmiLearningPackResolveLink(const UmiLearningPack *pack, const char *current,
    const char *href, UmiLearningPackLink *out)
{
    if (pack == NULL || current == NULL || href == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningPackLink candidate = {0};
    UmiStatus status = UmiLearningPackFindResource(pack, current, &candidate.resource);
    if (status != UMI_STATUS_OK) return status;
    size_t length = 0U;
    const size_t limit = UMI_TEACHER_PATH_CAPACITY + UMI_LEARNING_FRAGMENT_CAPACITY;
    while (length < limit && href[length] != '\0') ++length;
    if (length == limit) return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *fragment = strchr(href, '#');
    size_t path_size = fragment == NULL ? length : (size_t)(fragment - href);
    if (path_size >= UMI_TEACHER_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (fragment != NULL) {
        ++fragment;
        size_t fragment_size = length - (size_t)(fragment - href);
        if (fragment_size >= sizeof(candidate.fragment)) return UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t i = 0U; i < fragment_size; ++i)
            if (!NavigationAnchorCharacter((unsigned char)fragment[i]))
                return UMI_STATUS_INVALID_ARGUMENT;
        memcpy(candidate.fragment, fragment, fragment_size + 1U);
    }
    if (path_size != 0U) {
        if (href[0] == '/') return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t i = 0U; i < path_size; ++i) {
            unsigned char ch = (unsigned char)href[i];
            if (ch < 0x20U || ch == 0x7fU || ch == ':' || ch == '\\' || ch == '?' || ch == '%')
                return UMI_STATUS_INVALID_ARGUMENT;
        }
        char relative[UMI_TEACHER_PATH_CAPACITY];
        memcpy(relative, candidate.resource.path, sizeof(relative));
        NavigationSlashes(relative);
        char *last_slash = strrchr(relative, '/');
        size_t prefix = last_slash == NULL ? 0U : (size_t)(last_slash - relative) + 1U;
        if (path_size >= sizeof(relative) - prefix) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(relative + prefix, href, path_size);
        relative[prefix + path_size] = '\0';
        status = UmiLearningPackFindPath(pack, relative, &candidate.resource);
        if (status != UMI_STATUS_OK) return status;
    }
    /* Commit the complete selection only after path and anchor admission.
     * A reader can now request verified bytes by this resource ID. */
    *out = candidate;
    return UMI_STATUS_OK;
}
