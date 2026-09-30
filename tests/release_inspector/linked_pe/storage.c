/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_inspector/linked_pe/storage.c
 * PURPOSE:
 *   Freestanding PE format fixture, not a Windows application or storage engine. Clang/LLD
 *   produce genuine import/export records without a Windows SDK. This test image is
 *   inspected as bytes; it is never executed.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Foundation | Sammy Hegab | MIT
 * Freestanding PE format fixture, not a Windows application or storage engine.
 * Clang/LLD produce genuine import/export records without a Windows SDK.
 * This test image is inspected as bytes; it is never executed. */
__declspec(dllexport) int UmiPracticeValue(void) { return 23; }
int DllMain(void *module, unsigned reason, void *reserved)
{
    (void)module; (void)reason; (void)reserved;
    return 1;
}
