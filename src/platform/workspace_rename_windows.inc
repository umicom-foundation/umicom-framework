/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/workspace_rename_windows.inc
 *
 * PURPOSE:
 *   Rename through a held Windows source handle while retaining unlinked parent identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static UmiStatus RenameError(DWORD error) {
  if (error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS)
    return UMI_STATUS_ALREADY_EXISTS;
  if (error == ERROR_PATH_NOT_FOUND || error == ERROR_FILE_NOT_FOUND)
    return UMI_STATUS_NOT_FOUND;
  if (error == ERROR_ACCESS_DENIED || error == ERROR_SHARING_VIOLATION)
    return UMI_STATUS_PERMISSION_DENIED;
  if (error == ERROR_NOT_SUPPORTED || error == ERROR_INVALID_FUNCTION)
    return UMI_STATUS_NOT_IMPLEMENTED;
  return UMI_STATUS_IO_ERROR;
}
static UmiStatus RenameParent(const wchar_t *path, HANDLE *out) {
  *out =
      CreateFileW(path, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                  OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
  if (*out == INVALID_HANDLE_VALUE)
    return RenameError(GetLastError());
  BY_HANDLE_FILE_INFORMATION info;
  if (!GetFileInformationByHandle(*out, &info))
    return RenameError(GetLastError());
  if ((info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U ||
      (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0U)
    return UMI_STATUS_PERMISSION_DENIED;
  return UMI_STATUS_OK;
}
static UmiStatus RenameNative(const char *root, const char *relative,
                              UmiWorkspaceRenameResult *result) {
  (void)relative;
  wchar_t source[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY];
  HANDLE parents[UMI_PATH_CAPACITY / 2U], entry = INVALID_HANDLE_VALUE;
  size_t held = 0U;
  FILE_RENAME_INFO *renameInfo = NULL;
  int rootLength =
      MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, root, -1, source, (int)UMI_PATH_CAPACITY);
  if (rootLength <= 0)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = RenameParent(source, &parents[held]);
  if (parents[held] != INVALID_HANDLE_VALUE)
    ++held;
  if (status != UMI_STATUS_OK)
    goto done;
  if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, result->source, -1, source,
                          (int)UMI_PATH_CAPACITY) <= 0) {
    status = UMI_STATUS_INVALID_ARGUMENT;
    goto done;
  }
  for (size_t i = (size_t)rootLength; source[i] != L'\0'; ++i) {
    if (source[i] == L'/' || source[i] == L'\\') {
      if (held == sizeof parents / sizeof parents[0]) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
      }
      wchar_t separator = source[i];
      source[i] = L'\0';
      status = RenameParent(source, &parents[held]);
      source[i] = separator;
      if (parents[held] != INVALID_HANDLE_VALUE)
        ++held;
      if (status != UMI_STATUS_OK)
        goto done;
    }
  }
  entry =
      CreateFileW(source, DELETE | FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                  OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
  if (entry == INVALID_HANDLE_VALUE) {
    status = RenameError(GetLastError());
    goto done;
  }
  BY_HANDLE_FILE_INFORMATION info;
  if (!GetFileInformationByHandle(entry, &info)) {
    status = RenameError(GetLastError());
    goto done;
  }
  if ((info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U) {
    status = UMI_STATUS_PERMISSION_DENIED;
    goto done;
  }
  result->directory = (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0U;
  int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, result->destination, -1,
                                   destination, (int)UMI_PATH_CAPACITY);
  if (length <= 0) {
    status = UMI_STATUS_INVALID_ARGUMENT;
    goto done;
  }
  /* Supply native separators to the handle API. Its no-replacement flag,
   * not this early diagnostic check, protects against destination races. */
  for (int i = 0; i < length; ++i)
    if (destination[i] == L'/')
      destination[i] = L'\\';
  if (GetFileAttributesW(destination) != INVALID_FILE_ATTRIBUTES) {
    status = UMI_STATUS_ALREADY_EXISTS;
    goto done;
  }
  size_t bytes = (size_t)(length - 1) * sizeof(wchar_t);
  size_t storage = sizeof(FILE_RENAME_INFO) + bytes;
  renameInfo = calloc(1U, storage);
  if (renameInfo == NULL) {
    status = UMI_STATUS_OUT_OF_MEMORY;
    goto done;
  }
  renameInfo->ReplaceIfExists = FALSE;
  renameInfo->RootDirectory = NULL;
  renameInfo->FileNameLength = (DWORD)bytes;
  memcpy(renameInfo->FileName, destination, bytes);
  if (!SetFileInformationByHandle(entry, FileRenameInfo, renameInfo, (DWORD)storage)) {
    DWORD error = GetLastError();
    status = RenameError(error);
    if (status == UMI_STATUS_PERMISSION_DENIED &&
        GetFileAttributesW(destination) != INVALID_FILE_ATTRIBUTES)
      status = UMI_STATUS_ALREADY_EXISTS;
  } else
    result->renamed = 1;
done:
  free(renameInfo);
  if (entry != INVALID_HANDLE_VALUE && !CloseHandle(entry) && status == UMI_STATUS_OK)
    status = UMI_STATUS_IO_ERROR;
  while (held != 0U)
    if (!CloseHandle(parents[--held]) && status == UMI_STATUS_OK)
      status = UMI_STATUS_IO_ERROR;
  return status;
}
