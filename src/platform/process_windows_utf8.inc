/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_windows_utf8.inc
 * PURPOSE: Strict UTF-8 conversion, Unicode environments and private handle lists.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* All public text remains UTF-8. CreateProcessW requires writable UTF-16. */
static UmiStatus UmiWindowsUtf16(const char *text, wchar_t **out)
{
    *out = NULL;
    if (text == NULL) return UMI_STATUS_OK;
    size_t bytes = strlen(text);
    if (bytes > 4U * 32767U) return UMI_STATUS_CAPACITY_EXCEEDED;
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, NULL, 0);
    if (count <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > 32767) return UMI_STATUS_CAPACITY_EXCEEDED;
    wchar_t *value = malloc((size_t)count * sizeof(*value));
    if (value == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, value, count) != count) {
        free(value); return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out = value;
    return UMI_STATUS_OK;
}

/* Compare only the name and separator. Hidden drive records (=C:=...) remain
 * intact because public environment names cannot contain an equal sign. */
static int umi_windows_env_name_matches(const wchar_t *entry, const wchar_t *name)
{
    size_t length = wcslen(name);
    if (wcslen(entry) <= length || length > (size_t)INT_MAX) return 0;
    return entry[length] == L'=' &&
        CompareStringOrdinal(entry, (int)length, name, (int)length, TRUE) == CSTR_EQUAL;
}

static int UmiWindowsEnvironmentOrder(const void *left, const void *right)
{
    const wchar_t *a = *(const wchar_t *const *)left;
    const wchar_t *b = *(const wchar_t *const *)right;
    int compared = CompareStringOrdinal(a, -1, b, -1, TRUE);
    return compared == CSTR_LESS_THAN ? -1 : compared == CSTR_GREATER_THAN ? 1 : 0;
}

/* The returned block is sorted, double-NUL terminated and privately owned.
 * Duplicate overrides keep the last value, matching POSIX sequential setenv. */
static UmiStatus umi_windows_environment_block(const UmiProcessRequest *request, wchar_t **out)
{
    wchar_t *names[UMI_PROCESS_MAX_ENVIRONMENT] = {0};
    wchar_t *records[UMI_PROCESS_MAX_ENVIRONMENT] = {0};
    wchar_t **entries = NULL;
    wchar_t *current = NULL;
    size_t count = 0U, inherited = 0U, units = 1U;
    const size_t maximum = 2U * 1024U * 1024U;
    UmiStatus status = UMI_STATUS_OK;
    *out = NULL;
    if (request->environment_count == 0U) return status;
    for (size_t i = 0U; i < request->environment_count; ++i) {
        wchar_t *value = NULL;
        status = UmiWindowsUtf16(request->environment[i].name, &names[i]);
        if (status == UMI_STATUS_OK) status = UmiWindowsUtf16(request->environment[i].value, &value);
        if (status != UMI_STATUS_OK) { free(value); goto done; }
        size_t nameLength = wcslen(names[i]), valueLength = wcslen(value);
        records[i] = malloc((nameLength + valueLength + 2U) * sizeof(wchar_t));
        if (records[i] == NULL) { free(value); status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
        wmemcpy(records[i], names[i], nameLength);
        records[i][nameLength] = L'=';
        wmemcpy(records[i] + nameLength + 1U, value, valueLength + 1U);
        free(value);
    }
    current = GetEnvironmentStringsW();
    if (current == NULL) { status = UMI_STATUS_IO_ERROR; goto done; }
    for (wchar_t *entry = current; *entry != L'\0'; entry += wcslen(entry) + 1U) {
        size_t length = wcslen(entry) + 1U;
        if (length > maximum - units) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto done; }
        units += length;
        ++inherited;
    }
    entries = calloc(inherited + request->environment_count, sizeof(*entries));
    if (entries == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    for (wchar_t *entry = current; *entry != L'\0'; entry += wcslen(entry) + 1U) {
        int replaced = 0;
        for (size_t i = 0U; i < request->environment_count; ++i)
            if (umi_windows_env_name_matches(entry, names[i])) replaced = 1;
        if (!replaced) entries[count++] = entry;
    }
    for (size_t i = 0U; i < request->environment_count; ++i) {
        int replaced = 0;
        for (size_t j = i + 1U; j < request->environment_count; ++j)
            if (CompareStringOrdinal(names[i], -1, names[j], -1, TRUE) == CSTR_EQUAL) replaced = 1;
        if (!replaced) entries[count++] = records[i];
    }
    qsort(entries, count, sizeof(*entries), UmiWindowsEnvironmentOrder);
    units = 1U;
    for (size_t i = 0U; i < count; ++i) {
        size_t length = wcslen(entries[i]) + 1U;
        if (!umi_process_size_add(units, length, &units) || units > maximum) {
            status = UMI_STATUS_CAPACITY_EXCEEDED; goto done;
        }
    }
    if (units < 2U) units = 2U;
    *out = calloc(units, sizeof(wchar_t));
    if (*out == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    size_t used = 0U;
    for (size_t i = 0U; i < count; ++i) {
        size_t length = wcslen(entries[i]) + 1U;
        wmemcpy(*out + used, entries[i], length);
        used += length;
    }
done:
    if (current != NULL) (void)FreeEnvironmentStringsW(current);
    for (size_t i = 0U; i < UMI_PROCESS_MAX_ENVIRONMENT; ++i) { free(names[i]); free(records[i]); }
    free(entries);
    return status;
}

/* Resolve before CreateProcess so an executable containing spaces is never
 * interpreted as several possible filenames. Prefer explicit tool paths. */
static UmiStatus UmiWindowsProgram(const UmiProcessRequest *request, wchar_t **out)
{
    wchar_t *program = NULL, *path = NULL;
    UmiStatus status = UmiWindowsUtf16(request->program, &program);
    *out = NULL;
    if (status != UMI_STATUS_OK) return status;
    if (wcschr(program, L'\\') == NULL && wcschr(program, L'/') == NULL && wcschr(program, L':') == NULL) {
        for (size_t i = 0U; i < request->environment_count; ++i) {
            if (_stricmp(request->environment[i].name, "PATH") == 0) {
                free(path); path = NULL;
                status = UmiWindowsUtf16(request->environment[i].value, &path);
                if (status != UMI_STATUS_OK) goto done;
            }
        }
    }
    DWORD needed = SearchPathW(path, program, L".exe", 0U, NULL, NULL);
    if (needed == 0U) { status = UMI_STATUS_IO_ERROR; goto done; }
    if (needed >= 32767U) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto done; }
    *out = calloc((size_t)needed + 1U, sizeof(wchar_t));
    if (*out == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    DWORD written = SearchPathW(path, program, L".exe", needed + 1U, *out, NULL);
    if (written == 0U || written > needed) {
        free(*out); *out = NULL; status = UMI_STATUS_IO_ERROR;
    }
done:
    free(program); free(path);
    return status;
}

/* A GUI application may have no inherited console handle. NUL supplies a
 * valid endpoint without opening a console or borrowing an invalid handle. */
static HANDLE UmiWindowsStandardHandle(DWORD identifier, int input, SECURITY_ATTRIBUTES *security)
{
    HANDLE source = GetStdHandle(identifier), result = NULL;
    if (source != NULL && source != INVALID_HANDLE_VALUE &&
        DuplicateHandle(GetCurrentProcess(), source, GetCurrentProcess(), &result,
            0U, TRUE, DUPLICATE_SAME_ACCESS)) return result;
    result = CreateFileW(L"NUL", input ? GENERIC_READ : GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, security, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    return result != INVALID_HANDLE_VALUE ? result : NULL;
}
