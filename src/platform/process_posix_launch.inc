/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_posix_launch.inc
 * PURPOSE: Prepare argv, PATH candidates and environment before a worker forks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

extern char **environ;

typedef struct UmiPosixLaunch {
    char **environment;
    size_t environmentCount;
    char **paths;
    size_t pathCount;
} UmiPosixLaunch;

static void UmiPosixLaunchClear(UmiPosixLaunch *launch)
{
    for (size_t i = 0U; i < launch->environmentCount; ++i) free(launch->environment[i]);
    for (size_t i = 0U; i < launch->pathCount; ++i) free(launch->paths[i]);
    free(launch->environment);
    free(launch->paths);
    memset(launch, 0, sizeof(*launch));
}

static int UmiPosixEnvironmentName(const char *entry, const char *name)
{
    size_t length = strlen(name);
    return strncmp(entry, name, length) == 0 && entry[length] == '=';
}

/* The child never calls malloc, getenv or setenv after fork. Applications must
 * serialise their own process-wide environment mutations during this copy. */
static UmiStatus UmiPosixLaunchPrepare(const UmiProcessRequest *request, UmiPosixLaunch *launch)
{
    size_t inherited = 0U;
    size_t bytes = 0U;
    const size_t maximumBytes = 4U * 1024U * 1024U;
    const char *path = NULL;
    char *defaultPath = NULL;
    memset(launch, 0, sizeof(*launch));
    if (environ != NULL) {
        for (; environ[inherited] != NULL; ++inherited) {
            size_t length = strlen(environ[inherited]) + 1U;
            if (length > maximumBytes - bytes) return UMI_STATUS_CAPACITY_EXCEEDED;
            bytes += length;
        }
    }
    launch->environment = calloc(inherited + request->environment_count + 1U, sizeof(char *));
    if (launch->environment == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < inherited; ++i) {
        int replaced = 0;
        for (size_t n = 0U; n < request->environment_count; ++n)
            if (UmiPosixEnvironmentName(environ[i], request->environment[n].name)) replaced = 1;
        if (!replaced) {
            char *copy = strdup(environ[i]);
            if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
            launch->environment[launch->environmentCount++] = copy;
        }
    }
    for (size_t n = 0U; n < request->environment_count; ++n) {
        int replaced = 0;
        for (size_t j = n + 1U; j < request->environment_count; ++j)
            if (strcmp(request->environment[n].name, request->environment[j].name) == 0) replaced = 1;
        if (replaced) continue; /* Match the previous sequential setenv contract. */
        size_t nameLength = strlen(request->environment[n].name);
        size_t valueLength = strlen(request->environment[n].value);
        if (nameLength > maximumBytes - 2U || valueLength > maximumBytes - nameLength - 2U ||
            nameLength + valueLength + 2U > maximumBytes - bytes)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        bytes += nameLength + valueLength + 2U;
        char *copy = malloc(nameLength + valueLength + 2U);
        if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        memcpy(copy, request->environment[n].name, nameLength);
        copy[nameLength] = '=';
        memcpy(copy + nameLength + 1U, request->environment[n].value, valueLength + 1U);
        launch->environment[launch->environmentCount++] = copy;
    }
    if (strchr(request->program, '/') != NULL) {
        launch->paths = calloc(1U, sizeof(char *));
        if (launch->paths == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        launch->paths[0] = strdup(request->program);
        if (launch->paths[0] == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        launch->pathCount = 1U;
        return UMI_STATUS_OK;
    }
    for (size_t i = 0U; i < launch->environmentCount; ++i)
        if (strncmp(launch->environment[i], "PATH=", 5U) == 0) path = launch->environment[i] + 5U;
    if (path == NULL) {
        size_t required = confstr(_CS_PATH, NULL, 0U);
        if (required == 0U || required > maximumBytes) return UMI_STATUS_IO_ERROR;
        defaultPath = malloc(required);
        if (defaultPath == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        if (confstr(_CS_PATH, defaultPath, required) != required) {
            free(defaultPath); return UMI_STATUS_IO_ERROR;
        }
        path = defaultPath;
    }
    size_t count = 1U;
    for (const char *cursor = path; *cursor != '\0'; ++cursor) if (*cursor == ':') ++count;
    if (count > 4096U) { free(defaultPath); return UMI_STATUS_CAPACITY_EXCEEDED; }
    launch->paths = calloc(count, sizeof(char *));
    if (launch->paths == NULL) { free(defaultPath); return UMI_STATUS_OUT_OF_MEMORY; }
    const char *start = path;
    const size_t programLength = strlen(request->program);
    for (;;) {
        const char *end = strchr(start, ':');
        size_t prefix = end != NULL ? (size_t)(end - start) : strlen(start);
        if (prefix > maximumBytes - 2U || programLength > maximumBytes - prefix - 2U) {
            free(defaultPath); return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        char *candidate = malloc(prefix + programLength + 2U);
        if (candidate == NULL) { free(defaultPath); return UMI_STATUS_OUT_OF_MEMORY; }
        memcpy(candidate, start, prefix);
        size_t used = prefix;
        if (prefix != 0U) candidate[used++] = '/';
        memcpy(candidate + used, request->program, programLength + 1U);
        launch->paths[launch->pathCount++] = candidate;
        if (end == NULL) break;
        start = end + 1;
    }
    free(defaultPath);
    return UMI_STATUS_OK;
}

/* Keep internal descriptors above stdin/stdout/stderr, even for GUI hosts or
 * test processes whose standard descriptors have already been closed. */
static int UmiPosixPipe(int descriptors[2])
{
#if defined(__linux__)
    if (pipe2(descriptors, O_CLOEXEC) != 0) return 0;
#else
    if (pipe(descriptors) != 0) return 0;
#endif
    for (size_t i = 0U; i < 2U; ++i) {
        if (descriptors[i] <= STDERR_FILENO) {
            int replacement = fcntl(descriptors[i], F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
            if (replacement < 0) goto failed;
            (void)close(descriptors[i]);
            descriptors[i] = replacement;
        }
        if (fcntl(descriptors[i], F_SETFD, FD_CLOEXEC) != 0) goto failed;
    }
    return 1;
failed:
    (void)close(descriptors[0]); (void)close(descriptors[1]);
    descriptors[0] = descriptors[1] = -1;
    return 0;
}

/* Only async-signal-safe operations are used on this child-side path. */
static void UmiPosixChildFail(int descriptor, int error)
{
    const char *bytes = (const char *)&error;
    size_t left = sizeof(error);
    while (left != 0U) {
        ssize_t written = write(descriptor, bytes, left);
        if (written > 0) { bytes += written; left -= (size_t)written; }
        else if (written < 0 && errno == EINTR) continue;
        else break;
    }
    _exit(error == ENOENT ? 127 : 126);
}
