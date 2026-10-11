/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/site_preview.c
 *
 * PURPOSE:
 *   Stream an already-built educational website over loopback HTTP/1.1. This
 *   optional educational preview differs from the existing bounded-response
 *   Umicom Web Server: generated MkDocs pages and PDFs can exceed its 16 KiB
 *   response-body limit, so they need incremental file transmission.
 *
 *   Requests are read-only, one at a time. The server refuses directory
 *   listings, dot-files, traversal and symlinks/reparse points. It is not a
 *   public hosting, authentication or TLS component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/education/site.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
# define EDUCATION_SOCKET SOCKET
# define EDUCATION_SOCKLEN int
# define EDUCATION_INVALID_SOCKET INVALID_SOCKET
# define EDUCATION_CLOSE_SOCKET closesocket
# define EDUCATION_SHUTDOWN_RECEIVE SD_RECEIVE
#else
# include <arpa/inet.h>
# include <fcntl.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>
# define EDUCATION_SOCKET int
# define EDUCATION_SOCKLEN socklen_t
# define EDUCATION_INVALID_SOCKET (-1)
# define EDUCATION_CLOSE_SOCKET close
# define EDUCATION_SHUTDOWN_RECEIVE SHUT_RD
# if defined(MSG_NOSIGNAL)
#  define EDUCATION_SEND_FLAGS MSG_NOSIGNAL
# else
#  define EDUCATION_SEND_FLAGS 0
# endif
#endif

#define EDUCATION_REQUEST_CAPACITY 8192U
#define EDUCATION_TRANSFER_CAPACITY 16384U

static volatile sig_atomic_t educationStopRequested = 0;

static void EducationStopOnSignal(int signalNumber)
{
    (void)signalNumber;
    educationStopRequested = 1;
}

static int EducationHexDigit(unsigned char value)
{
    if (value >= (unsigned char)'0' && value <= (unsigned char)'9')
        return (int)(value - (unsigned char)'0');
    if (value >= (unsigned char)'a' && value <= (unsigned char)'f')
        return (int)(value - (unsigned char)'a' + 10U);
    if (value >= (unsigned char)'A' && value <= (unsigned char)'F')
        return (int)(value - (unsigned char)'A' + 10U);
    return -1;
}

/* Windows removes trailing dots/spaces and interprets special device names.
 * Reject those names for every host so test cases mean the same thing. */
static int EducationValidSegment(const char *segment, size_t length)
{
    if (length == 0U || segment[0] == '.' || segment[length - 1U] == '.' ||
        segment[length - 1U] == ' ') return 0;
    size_t base = 0U;
    while (base < length && segment[base] != '.') ++base;
    if (base == 3U) {
        char name[4];
        for (size_t i = 0U; i < 3U; ++i)
            name[i] = (char)toupper((unsigned char)segment[i]);
        name[3] = '\0';
        if (strcmp(name, "CON") == 0 || strcmp(name, "PRN") == 0 ||
            strcmp(name, "AUX") == 0 || strcmp(name, "NUL") == 0) return 0;
    }
    if (base == 4U) {
        char prefix[4];
        for (size_t i = 0U; i < 3U; ++i)
            prefix[i] = (char)toupper((unsigned char)segment[i]);
        prefix[3] = '\0';
        if ((strcmp(prefix, "COM") == 0 || strcmp(prefix, "LPT") == 0) &&
            segment[3] >= '1' && segment[3] <= '9') return 0;
    }
    return 1;
}

UmicomEducationPathStatus UmicomEducationResolveUrlPath(
    const char *urlTarget, char *relative, size_t capacity)
{
    if (urlTarget == NULL || relative == NULL || capacity < 12U || urlTarget[0] != '/')
        return UMICOM_EDUCATION_PATH_INVALID;

    size_t used = 0U;
    size_t segmentStart = 0U;
    size_t i = 1U;
    /* Bound source scanning separately from the destination capacity. */
    for (; i < UMICOM_EDUCATION_PATH_CAPACITY; ++i) {
        unsigned char value = (unsigned char)urlTarget[i];
        if (value == '\0' || value == '?')
            break;
        if (value == '%') {
            /* Check termination before reading the second hexadecimal digit.
             * A caller may supply only a two-byte string such as "/%". */
            if (urlTarget[i + 1U] == '\0')
                return UMICOM_EDUCATION_PATH_INVALID;
            if (urlTarget[i + 2U] == '\0')
                return UMICOM_EDUCATION_PATH_INVALID;
            int high = EducationHexDigit((unsigned char)urlTarget[i + 1U]);
            if (high < 0) return UMICOM_EDUCATION_PATH_INVALID;
            int low = EducationHexDigit((unsigned char)urlTarget[i + 2U]);
            if (low < 0)
                return UMICOM_EDUCATION_PATH_INVALID;
            value = (unsigned char)((unsigned)high * 16U + (unsigned)low);
            i += 2U;
            /* Reinterpreting a decoded slash or percent can bypass checking. */
            if (value == '/' || value == '\\' || value == '%')
                return UMICOM_EDUCATION_PATH_INVALID;
        }
        if (value == '\\' || value == ':' || value == '%' || value == '#' ||
            value < 0x20U || value == 0x7fU)
            return UMICOM_EDUCATION_PATH_INVALID;
        if (value == '/') {
            if (used == segmentStart)
                return UMICOM_EDUCATION_PATH_INVALID;
            /* Dot-prefixed names, including .git, are never exposed. */
            if (!EducationValidSegment(relative + segmentStart, used - segmentStart))
                return UMICOM_EDUCATION_PATH_INVALID;
            if (used + 1U >= capacity)
                return UMICOM_EDUCATION_PATH_TOO_LONG;
            relative[used++] = '/';
            segmentStart = used;
        } else {
            if (used + 1U >= capacity)
                return UMICOM_EDUCATION_PATH_TOO_LONG;
            relative[used++] = (char)value;
        }
    }
    if (i == UMICOM_EDUCATION_PATH_CAPACITY)
        return UMICOM_EDUCATION_PATH_TOO_LONG;
    if (used > segmentStart && !EducationValidSegment(relative + segmentStart, used - segmentStart))
        return UMICOM_EDUCATION_PATH_INVALID;
    if (used == segmentStart) {
        static const char indexName[] = "index.html";
        if (capacity - used < sizeof(indexName))
            return UMICOM_EDUCATION_PATH_TOO_LONG;
        memcpy(relative + used, indexName, sizeof(indexName));
    } else {
        relative[used] = '\0';
    }
    return UMICOM_EDUCATION_PATH_OK;
}

static int EducationCaseEqual(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        ++a;
        ++b;
    }
    return *a == *b;
}

const char *UmicomEducationMimeType(const char *relative)
{
    const char *extension = strrchr(relative, '.');
    if (extension == NULL) return "application/octet-stream";
    if (EducationCaseEqual(extension, ".html") || EducationCaseEqual(extension, ".htm"))
        return "text/html; charset=utf-8";
    if (EducationCaseEqual(extension, ".css")) return "text/css; charset=utf-8";
    if (EducationCaseEqual(extension, ".js") || EducationCaseEqual(extension, ".mjs"))
        return "text/javascript; charset=utf-8";
    if (EducationCaseEqual(extension, ".json") || EducationCaseEqual(extension, ".map"))
        return "application/json; charset=utf-8";
    if (EducationCaseEqual(extension, ".svg")) return "image/svg+xml";
    if (EducationCaseEqual(extension, ".png")) return "image/png";
    if (EducationCaseEqual(extension, ".jpg") || EducationCaseEqual(extension, ".jpeg")) return "image/jpeg";
    if (EducationCaseEqual(extension, ".webp")) return "image/webp";
    if (EducationCaseEqual(extension, ".gif")) return "image/gif";
    if (EducationCaseEqual(extension, ".ico")) return "image/x-icon";
    if (EducationCaseEqual(extension, ".woff2")) return "font/woff2";
    if (EducationCaseEqual(extension, ".woff")) return "font/woff";
    if (EducationCaseEqual(extension, ".ttf")) return "font/ttf";
    if (EducationCaseEqual(extension, ".xml")) return "application/xml";
    if (EducationCaseEqual(extension, ".pdf")) return "application/pdf";
    if (EducationCaseEqual(extension, ".zip")) return "application/zip";
    if (EducationCaseEqual(extension, ".txt") || EducationCaseEqual(extension, ".md"))
        return "text/plain; charset=utf-8";
    return "application/octet-stream";
}

static int EducationSendAll(EDUCATION_SOCKET socketHandle, const unsigned char *bytes, size_t count)
{
    size_t sent = 0U;
    while (sent < count) {
        size_t portion = count - sent;
        if (portion > 16384U) portion = 16384U;
#if defined(_WIN32)
        int result = send(socketHandle, (const char *)bytes + sent, (int)portion, 0);
#else
        ssize_t result = send(socketHandle, (const void *)(bytes + sent), portion, EDUCATION_SEND_FLAGS);
#endif
        if (result <= 0) return 0;
        sent += (size_t)result;
    }
    return 1;
}

static void EducationSendStatus(EDUCATION_SOCKET socketHandle, int status,
                                const char *reason, int omitBody)
{
    char header[600];
    char body[128];
    int length = snprintf(body, sizeof(body), "%d %s\n", status, reason);
    if (length <= 0 || (size_t)length >= sizeof(body)) return;
    int headerLength = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\nContent-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: %d\r\nConnection: close\r\n"
        "X-Content-Type-Options: nosniff\r\nCache-Control: no-store\r\n\r\n",
        status, reason, length);
    if (headerLength <= 0 || (size_t)headerLength >= sizeof(header)) return;
    if (EducationSendAll(socketHandle, (const unsigned char *)header, (size_t)headerLength) && !omitBody)
        (void)EducationSendAll(socketHandle, (const unsigned char *)body, (size_t)length);
}

typedef struct EducationOpenedFile {
#if defined(_WIN32)
    HANDLE handle;
#else
    int descriptor;
#endif
    uint64_t length;
} EducationOpenedFile;

static void EducationCloseFile(EducationOpenedFile *file)
{
#if defined(_WIN32)
    if (file->handle != INVALID_HANDLE_VALUE) CloseHandle(file->handle);
    file->handle = INVALID_HANDLE_VALUE;
#else
    if (file->descriptor >= 0) close(file->descriptor);
    file->descriptor = -1;
#endif
}

/* Return 200, 403, 404, or 500. An opened result must be closed exactly once. */
static int EducationOpenFile(const char *root, const char *relative, EducationOpenedFile *out)
{
#if defined(_WIN32)
    /* Reject reparse points at every path component, not just the last one.
     * Local Windows preview still assumes the trusted build tree is not being
     * concurrently rewritten by an adversary (no NT relative-handle walk). */
    char current[UMICOM_EDUCATION_PATH_CAPACITY];
    int n = snprintf(current, sizeof(current), "%s", root);
    if (n < 0 || (size_t)n >= sizeof(current)) return 403;
    DWORD attrs = GetFileAttributesA(current);
    if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY) ||
        (attrs & FILE_ATTRIBUTE_REPARSE_POINT)) return 403;
    const char *segment = relative;
    while (*segment != '\0') {
        const char *end = strchr(segment, '/');
        size_t length = end == NULL ? strlen(segment) : (size_t)(end - segment);
        size_t currentLength = strlen(current);
        if (currentLength + length + 2U >= sizeof(current)) return 403;
        current[currentLength++] = '\\';
        memcpy(current + currentLength, segment, length);
        current[currentLength + length] = '\0';
        attrs = GetFileAttributesA(current);
        if (attrs == INVALID_FILE_ATTRIBUTES) return 404;
        if ((attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0U) return 403;
        if (end != NULL && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) return 404;
        if (end == NULL && (attrs & FILE_ATTRIBUTE_DIRECTORY)) return 404;
        segment = end == NULL ? segment + length : end + 1;
    }
    out->handle = CreateFileA(current, GENERIC_READ, FILE_SHARE_READ,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (out->handle == INVALID_HANDLE_VALUE) return 404;
    BY_HANDLE_FILE_INFORMATION info;
    if (!GetFileInformationByHandle(out->handle, &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT))) {
        EducationCloseFile(out);
        return 403;
    }
    LARGE_INTEGER size;
    if (!GetFileSizeEx(out->handle, &size) || size.QuadPart < 0) {
        EducationCloseFile(out);
        return 500;
    }
    out->length = (uint64_t)size.QuadPart;
    return 200;
#else
    int current = open(root, O_RDONLY | O_DIRECTORY | O_NOFOLLOW);
    if (current < 0) return errno == ENOENT ? 404 : 403;
    const char *segment = relative;
    for (;;) {
        const char *end = strchr(segment, '/');
        size_t length = end == NULL ? strlen(segment) : (size_t)(end - segment);
        char component[UMICOM_EDUCATION_PATH_CAPACITY];
        if (length == 0U || length >= sizeof(component)) {
            close(current);
            return 403;
        }
        memcpy(component, segment, length);
        component[length] = '\0';
        /* Classify a link as forbidden, while openat(O_NOFOLLOW) remains the
         * authoritative guard against a concurrent symlink replacement. */
#if defined(AT_SYMLINK_NOFOLLOW)
        struct stat entry;
        if (fstatat(current, component, &entry, AT_SYMLINK_NOFOLLOW) == 0 &&
            S_ISLNK(entry.st_mode)) {
            close(current);
            return 403;
        }
#endif
        int flags = O_RDONLY | O_NOFOLLOW | (end != NULL ? O_DIRECTORY : O_NONBLOCK);
        int next = openat(current, component, flags);
        if (next < 0) {
            int errorNumber = errno;
            close(current);
            return errorNumber == ENOENT ? 404 : (errorNumber == ELOOP || errorNumber == EACCES ? 403 : 404);
        }
        close(current);
        current = next;
        if (end == NULL) break;
        segment = end + 1;
    }
    struct stat info;
    if (fstat(current, &info) != 0 || !S_ISREG(info.st_mode) || info.st_size < 0) {
        close(current);
        return 403;
    }
    out->length = (uint64_t)info.st_size;
    out->descriptor = current;
    return 200;
#endif
}

static int EducationWriteFile(EDUCATION_SOCKET socketHandle, EducationOpenedFile *file)
{
    unsigned char buffer[EDUCATION_TRANSFER_CAPACITY];
    for (;;) {
#if defined(_WIN32)
        DWORD count = 0U;
        if (!ReadFile(file->handle, buffer, (DWORD)sizeof(buffer), &count, NULL)) return 0;
        if (count == 0U) return 1;
        if (!EducationSendAll(socketHandle, buffer, (size_t)count)) return 0;
#else
        ssize_t count = read(file->descriptor, buffer, sizeof(buffer));
        if (count < 0 && errno == EINTR) continue;
        if (count < 0) return 0;
        if (count == 0) return 1;
        if (!EducationSendAll(socketHandle, buffer, (size_t)count)) return 0;
#endif
    }
}

static int EducationHeaderNameMatches(const char *line, size_t length, const char *name)
{
    size_t nameLength = strlen(name);
    if (length < nameLength + 1U || line[nameLength] != ':') return 0;
    for (size_t i = 0U; i < nameLength; ++i)
        if (tolower((unsigned char)line[i]) != tolower((unsigned char)name[i])) return 0;
    return 1;
}

static int EducationValidateHost(const char *request, size_t port)
{
    /* Validate the literal Host independently of loopback socket binding.
     * This also makes DNS rebinding through an unrelated hostname ineffective. */
    char allowedIP[40];
    char allowedLocal[40];
    (void)snprintf(allowedIP, sizeof(allowedIP), "127.0.0.1:%u", (unsigned)port);
    (void)snprintf(allowedLocal, sizeof(allowedLocal), "localhost:%u", (unsigned)port);
    const char *line = strstr(request, "\r\n");
    int sawHost = 0;
    if (line == NULL) return 0;
    line += 2;
    while (!(line[0] == '\r' && line[1] == '\n')) {
        const char *end = strstr(line, "\r\n");
        if (end == NULL) return 0;
        size_t length = (size_t)(end - line);
        if (EducationHeaderNameMatches(line, length, "Host")) {
            if (sawHost) return 0;
            sawHost = 1;
            const char *host = line + 5;
            while (host < end && (*host == ' ' || *host == '\t')) ++host;
            size_t hostLength = (size_t)(end - host);
            if (!((strlen(allowedIP) == hostLength && strncmp(host, allowedIP, hostLength) == 0) ||
                  (strlen(allowedLocal) == hostLength && strncmp(host, allowedLocal, hostLength) == 0)))
                return 0;
        } else if (EducationHeaderNameMatches(line, length, "Origin")) {
            const char *origin = line + 7;
            while (origin < end && (*origin == ' ' || *origin == '\t')) ++origin;
            char allowedOriginIP[64];
            char allowedOriginLocal[64];
            (void)snprintf(allowedOriginIP, sizeof(allowedOriginIP), "http://%s", allowedIP);
            (void)snprintf(allowedOriginLocal, sizeof(allowedOriginLocal), "http://%s", allowedLocal);
            size_t originLength = (size_t)(end - origin);
            if (!((strlen(allowedOriginIP) == originLength && strncmp(origin, allowedOriginIP, originLength) == 0) ||
                  (strlen(allowedOriginLocal) == originLength && strncmp(origin, allowedOriginLocal, originLength) == 0)))
                return 0;
        }
        line = end + 2;
    }
    return sawHost;
}

static void EducationHandleClient(EDUCATION_SOCKET client, const char *root, uint16_t port)
{
    char request[EDUCATION_REQUEST_CAPACITY];
    size_t used = 0U;
    while (used + 1U < sizeof(request)) {
#if defined(_WIN32)
        int count = recv(client, request + used, (int)(sizeof(request) - used - 1U), 0);
#else
        ssize_t count = recv(client, request + used, sizeof(request) - used - 1U, 0);
#endif
        if (count <= 0) return;
        used += (size_t)count;
        request[used] = '\0';
        if (strstr(request, "\r\n\r\n") != NULL) break;
    }
    if (strstr(request, "\r\n\r\n") == NULL) {
        EducationSendStatus(client, 431, "Request Header Fields Too Large", 0);
        return;
    }
    char method[12] = {0};
    char target[UMICOM_EDUCATION_PATH_CAPACITY] = {0};
    char version[12] = {0};
    if (sscanf(request, "%11s %4095s %11s", method, target, version) != 3 ||
        strcmp(version, "HTTP/1.1") != 0) {
        EducationSendStatus(client, 400, "Bad Request", 0);
        return;
    }
    if (!EducationValidateHost(request, port)) {
        EducationSendStatus(client, 403, "Forbidden", strcmp(method, "HEAD") == 0);
        return;
    }
    int isHead = strcmp(method, "HEAD") == 0;
    if (strcmp(method, "GET") != 0 && !isHead) {
        EducationSendStatus(client, 405, "Method Not Allowed", 0);
        return;
    }
    char relative[UMICOM_EDUCATION_PATH_CAPACITY];
    if (UmicomEducationResolveUrlPath(target, relative, sizeof(relative)) != UMICOM_EDUCATION_PATH_OK) {
        EducationSendStatus(client, 403, "Forbidden", strcmp(method, "HEAD") == 0);
        return;
    }
    EducationOpenedFile file;
    memset(&file, 0, sizeof(file));
#if defined(_WIN32)
    file.handle = INVALID_HANDLE_VALUE;
#else
    file.descriptor = -1;
#endif
    int code = EducationOpenFile(root, relative, &file);
    if (code != 200) {
        EducationSendStatus(client, code, code == 404 ? "Not Found" : (code == 403 ? "Forbidden" : "Internal Server Error"), isHead);
        return;
    }
    char header[900];
    int headerLength = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %" PRIu64 "\r\n"
        "Connection: close\r\nCache-Control: no-store\r\nX-Content-Type-Options: nosniff\r\n"
        "Content-Security-Policy: default-src 'self' data:; object-src 'none'; base-uri 'self'; frame-ancestors 'none'\r\n\r\n",
        UmicomEducationMimeType(relative), file.length);
    if (headerLength > 0 && (size_t)headerLength < sizeof(header) &&
        EducationSendAll(client, (const unsigned char *)header, (size_t)headerLength) && !isHead)
        (void)EducationWriteFile(client, &file);
    EducationCloseFile(&file);
}

int UmicomEducationServeSite(const char *siteRoot, uint16_t port, int once)
{
    if (siteRoot == NULL || *siteRoot == '\0' || port < 1024U)
        return 2;
#if defined(_WIN32)
    /* All filesystem operations are local developer-only preview operations.
     * Windows callers should use native ASCII project paths in this edition;
     * the Unicode CreateFileW adapter is a follow-on interoperability step. */
    DWORD rootAttributes = GetFileAttributesA(siteRoot);
    if (rootAttributes == INVALID_FILE_ATTRIBUTES || !(rootAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
        (rootAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
        fputs("The preview directory does not exist or is a reparse point.\n", stderr);
        return 2;
    }
    WSADATA winsock;
    if (WSAStartup(MAKEWORD(2, 2), &winsock) != 0) return 1;
#else
    struct stat rootStat;
    if (lstat(siteRoot, &rootStat) != 0 || !S_ISDIR(rootStat.st_mode)) {
        fputs("The preview directory does not exist or is a symbolic link.\n", stderr);
        return 2;
    }
#endif
    EDUCATION_SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == EDUCATION_INVALID_SOCKET) {
#if defined(_WIN32)
        WSACleanup();
#endif
        return 1;
    }
    int reuse = 1;
#if defined(_WIN32)
    (void)setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));
#else
    (void)setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(listener, (const struct sockaddr *)&address, (EDUCATION_SOCKLEN)sizeof(address)) != 0 ||
        listen(listener, 8) != 0) {
        fprintf(stderr, "Cannot bind 127.0.0.1:%u (port occupied or unavailable).\n", (unsigned)port);
        EDUCATION_CLOSE_SOCKET(listener);
#if defined(_WIN32)
        WSACleanup();
#endif
        return 1;
    }
    educationStopRequested = 0;
    (void)signal(SIGINT, EducationStopOnSignal);
    (void)signal(SIGTERM, EducationStopOnSignal);
    printf("Serving read-only site at http://127.0.0.1:%u/\nFolder: %s\nStop with Ctrl+C.\n",
           (unsigned)port, siteRoot);
    fflush(stdout);
    unsigned served = 0U;
    int result = 0;
    while (!educationStopRequested) {
        fd_set readable;
        FD_ZERO(&readable);
        FD_SET(listener, &readable);
        struct timeval waitTime;
        waitTime.tv_sec = 0;
        waitTime.tv_usec = 250000;
#if defined(_WIN32)
        int selected = select(0, &readable, NULL, NULL, &waitTime);
#else
        int selected = select(listener + 1, &readable, NULL, NULL, &waitTime);
#endif
        if (selected < 0) {
#if !defined(_WIN32)
            if (errno == EINTR) continue;
#endif
            result = 1;
            break;
        }
        if (selected == 0) continue;
        EDUCATION_SOCKET client = accept(listener, NULL, NULL);
        if (client == EDUCATION_INVALID_SOCKET) continue;
#if defined(_WIN32)
        DWORD timeoutMillis = 3000U;
        (void)setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeoutMillis, sizeof(timeoutMillis));
        (void)setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeoutMillis, sizeof(timeoutMillis));
#else
        struct timeval timeout = {3, 0};
        (void)setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        (void)setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
#endif
        EducationHandleClient(client, siteRoot, port);
        EDUCATION_CLOSE_SOCKET(client);
        ++served;
        if (once && served >= 1U) break;
    }
    EDUCATION_CLOSE_SOCKET(listener);
#if defined(_WIN32)
    WSACleanup();
#endif
    return result;
}
