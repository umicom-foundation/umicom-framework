/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_attestation_win32_names.c
 *
 * PURPOSE:
 *   Compile-only regression for Win32 names colliding with private C helpers.
 *
 * ENGINEERING RATIONALE:
 *   A function declared 'static' has internal linkage but shares the ordinary
 *   identifier namespace with declarations in included system headers. The
 *   Win32 ReadFile prototype therefore conflicts with a private helper named
 *   ReadFile even though that helper is not exported from the object file.
 *
 *   Compile the ACTUAL attestation implementation after the Windows API
 *   declaration, rather than testing a replacement algorithm or a loose
 *   copy of the helper. The generated object is deliberately never linked:
 *   normal production builds retain exactly one cryptography implementation.
 *   Non-Windows compilers use a signature-compatible declaration to catch
 *   this class of collision without relying on a Windows SDK installation.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#if defined(_WIN32)
#include <windows.h>
#else
/* Compile-only stand-in for the existing Win32 ReadFile prototype. */
extern int ReadFile(void *fileHandle, void *buffer,
                    unsigned long requestedBytes,
                    unsigned long *actualBytes, void *overlapped);
#endif

/* A compile-time probe, not a second linked implementation or test executable. */
#include "../../src/ai/training/training_attestation.c"
