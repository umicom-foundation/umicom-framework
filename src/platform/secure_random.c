/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/secure_random.c
 * PURPOSE: Use native random providers for private persistent identities and other caller-owned byte sequences.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/platform/secure_random.h"
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#elif defined(__linux__)
#include <errno.h>
#include <sys/random.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
UmiStatus UmiSecureRandomBytes(void *output, size_t bytes)
{
    if (bytes > UMI_SECURE_RANDOM_MAXIMUM_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (bytes == 0U)
        return UMI_STATUS_OK;
    if (output == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
#ifdef _WIN32
    if (BCryptGenRandom(NULL, output, (ULONG)bytes, BCRYPT_USE_SYSTEM_PREFERRED_RNG) < 0)
        status = UMI_STATUS_UNAVAILABLE;
#elif defined(__linux__)
    size_t done = 0U;
    while (done < bytes)
    {
        ssize_t count = getrandom((unsigned char *)output + done, bytes - done, 0);
        if (count < 0 && errno == EINTR)
            continue;
        if (count <= 0)
        {
            status = UMI_STATUS_UNAVAILABLE;
            break;
        }
        done += (size_t)count;
    }
#elif defined(__unix__) || defined(__APPLE__)
    int flags = O_RDONLY;
#ifdef O_CLOEXEC
    flags |= O_CLOEXEC;
#endif
#ifdef O_NOFOLLOW
    flags |= O_NOFOLLOW;
#endif
    int descriptor = open("/dev/urandom", flags);
    if (descriptor < 0)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
    {
        struct stat info;
        if (fstat(descriptor, &info) != 0 || !S_ISCHR(info.st_mode))
            status = UMI_STATUS_UNAVAILABLE;
        /* Set close-on-exec as well on platforms lacking O_CLOEXEC. The file
         * descriptor is private to this call and is never a child input. */
        if (status == UMI_STATUS_OK && fcntl(descriptor, F_SETFD, FD_CLOEXEC) < 0)
            status = UMI_STATUS_UNAVAILABLE;
        size_t done = 0U;
        while (status == UMI_STATUS_OK && done < bytes)
        {
            ssize_t count = read(descriptor, (unsigned char *)output + done, bytes - done);
            if (count < 0 && errno == EINTR)
                continue;
            if (count <= 0)
            {
                status = UMI_STATUS_UNAVAILABLE;
                break;
            }
            done += (size_t)count;
        }
        if (close(descriptor) != 0 && status == UMI_STATUS_OK)
            status = UMI_STATUS_IO_ERROR;
    }
#else
    status = UMI_STATUS_NOT_IMPLEMENTED;
#endif
    /* An unsuccessful call never leaves a partially usable identifier. */
    if (status != UMI_STATUS_OK)
        memset(output, 0, bytes);
    return status;
}
