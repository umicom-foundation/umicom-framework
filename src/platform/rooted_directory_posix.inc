/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/rooted_directory_posix.inc
 *
 * PURPOSE:
 *   Create missing relative directory components through held POSIX descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Preserve the ensure-only traversal for review. The shared traversal below
 * also supports an exclusive final component while keeping existing-parent
 * checks and partial progress in one native implementation. */
#if 0
static UmiStatus RootedEnsureDirectories(const char *root, const char *relative,
    RootedParent *parent, UmiRootedDirectoryCreation *report)
{
    parent->parent = open(root, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (parent->parent < 0) return RootedError(errno);
    memcpy(parent->parts, relative, strlen(relative) + 1U);
    char *component = parent->parts;
    for (;;) {
        char *separator = strchr(component, '/');
        if (separator != NULL) *separator = '\0';
        int next = openat(parent->parent, component,
            O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        if (next < 0 && errno == ENOENT) {
            /* EEXIST may be another creator or a link/file. Reopen with the
             * same no-follow directory contract; existence alone is not enough. */
            if (mkdirat(parent->parent, component, 0700) == 0)
                ++report->directories_created;
            else if (errno != EEXIST)
                return RootedError(errno);
            next = openat(parent->parent, component,
                O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        }
        if (next < 0) return RootedError(errno);
        int previous = parent->parent;
        parent->parent = next;
        /* Never retry close: the descriptor number may already be reusable. */
        if (close(previous) != 0) return UMI_STATUS_IO_ERROR;
        if (separator == NULL) return UMI_STATUS_OK;
        component = separator + 1;
    }
}
#endif

static UmiStatus RootedPrepareDirectories(const char *root, const char *relative,
    RootedParent *parent, UmiRootedDirectoryCreation *report, int exclusiveLeaf)
{
    parent->parent = open(root, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (parent->parent < 0) return RootedError(errno);
    memcpy(parent->parts, relative, strlen(relative) + 1U);
    char *component = parent->parts;
    for (;;) {
        char *separator = strchr(component, '/');
        if (separator != NULL) *separator = '\0';
        int next;
        if (exclusiveLeaf && separator == NULL) {
            /* Reservation belongs to mkdirat itself, not a prior existence query.
             * A competing ordinary directory or link is refused as EEXIST. */
            if (mkdirat(parent->parent, component, 0700) != 0)
                return RootedError(errno);
            ++report->directories_created;
            next = openat(parent->parent, component,
                O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        } else {
            next = openat(parent->parent, component,
                O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        }
        if (next < 0 && errno == ENOENT && !(exclusiveLeaf && separator == NULL)) {
            /* EEXIST may be another creator or a link/file. Reopen with the
             * same no-follow directory contract; existence alone is not enough. */
            if (mkdirat(parent->parent, component, 0700) == 0)
                ++report->directories_created;
            else if (errno != EEXIST)
                return RootedError(errno);
            next = openat(parent->parent, component,
                O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        }
        if (next < 0) return RootedError(errno);
        int previous = parent->parent;
        parent->parent = next;
        /* Never retry close: the descriptor number may already be reusable. */
        if (close(previous) != 0) return UMI_STATUS_IO_ERROR;
        if (separator == NULL) return UMI_STATUS_OK;
        component = separator + 1;
    }
}
static UmiStatus RootedDirectoryClose(RootedParent *parent, UmiStatus status)
{
    int descriptor = parent->parent;
    parent->parent = -1;
    if (descriptor >= 0 && close(descriptor) != 0 && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    return status;
}
