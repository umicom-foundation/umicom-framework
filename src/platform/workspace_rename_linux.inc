/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/workspace_rename_linux.inc
 *
 * PURPOSE:
 *   Use descriptor-relative Linux renameat2 with no destination replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static UmiStatus RenameError(int error) {
  if (error == EEXIST || error == ENOTEMPTY)
    return UMI_STATUS_ALREADY_EXISTS;
  if (error == ENOENT)
    return UMI_STATUS_NOT_FOUND;
  if (error == EACCES || error == EPERM || error == ELOOP || error == ENOTDIR)
    return UMI_STATUS_PERMISSION_DENIED;
  if (error == ENOSYS || error == EINVAL || error == EOPNOTSUPP)
    return UMI_STATUS_NOT_IMPLEMENTED;
  return UMI_STATUS_IO_ERROR;
}

/* Opening parents without following links avoids path traversal through a
 * replacement link. No fallback may unlink a source after copying it. */
static UmiStatus RenameNative(const char *root, const char *relative,
                              UmiWorkspaceRenameResult *result) {
#ifndef SYS_renameat2
  (void)root;
  (void)relative;
  (void)result;
  return UMI_STATUS_NOT_IMPLEMENTED;
#else
  char parts[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY];
  memcpy(parts, relative, strlen(relative) + 1U);
  UmiStatus status = umi_path_basename(result->destination, destination, sizeof destination);
  if (status != UMI_STATUS_OK)
    return status;
  int parent = open(root, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
  if (parent < 0)
    return RenameError(errno);
  char *name = parts, *separator;
  while ((separator = strchr(name, '/')) != NULL) {
    *separator = '\0';
    int next = openat(parent, name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (next < 0) {
      status = RenameError(errno);
      goto done;
    }
    (void)close(parent);
    parent = next;
    name = separator + 1;
  }
  struct stat info;
  if (fstatat(parent, name, &info, AT_SYMLINK_NOFOLLOW) != 0) {
    status = RenameError(errno);
    goto done;
  }
  if (S_ISLNK(info.st_mode)) {
    status = UMI_STATUS_PERMISSION_DENIED;
    goto done;
  }
  if (!S_ISREG(info.st_mode) && !S_ISDIR(info.st_mode)) {
    status = UMI_STATUS_INVALID_ARGUMENT;
    goto done;
  }
  result->directory = S_ISDIR(info.st_mode) != 0;
  /* RENAME_NOREPLACE is 1 in the Linux ABI. The destination check and move
   * are one syscall; an exists-then-rename sequence would permit overwrite. */
  if (syscall(SYS_renameat2, parent, name, parent, destination, 1U) != 0)
    status = RenameError(errno);
  else
    result->renamed = 1;
done:
  if (close(parent) != 0 && status == UMI_STATUS_OK)
    status = UMI_STATUS_IO_ERROR;
  return status;
#endif
}
