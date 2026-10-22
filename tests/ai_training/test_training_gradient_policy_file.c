/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_gradient_policy_file.c
 * PURPOSE: Check independent no-clobber companion-file publication and
 *          fail-closed tampering, truncation and missing-file behaviour.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/gradient_policy.h"
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    char path[128];
#if defined(_WIN32)
    const unsigned long processId = (unsigned long)GetCurrentProcessId();
#else
    const unsigned long processId = (unsigned long)getpid();
#endif
    const unsigned char checkpoint[] = {0xAAU, 0x00U, 0x55U};
    UmiAiTrainingGradientPolicy policy = {2.0F, 2U, 1U};
    UmiAiTrainingGradientPolicy changed = {4.0F, 2U, 1U};
    FILE *file;
    /* Never remove a pre-existing file in a shared CTest build directory.
     * Each process gets a fresh, dedicated output name, which must be free
     * before the no-clobber publication test can proceed. */
    const int nameLength = snprintf(path, sizeof(path),
        "umicom-gradient-policy-regression-%lu.policy", processId);
    CHECK(nameLength > 0 && (size_t)nameLength < sizeof(path));
    CHECK(UmiAiTrainingGradientPolicyVerifyFile(
        path, checkpoint, sizeof(checkpoint), &policy) == UMI_STATUS_IO_ERROR);
    CHECK(UmiAiTrainingGradientPolicySaveNewFile(
        path, checkpoint, sizeof(checkpoint), &policy) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingGradientPolicySaveNewFile(
        path, checkpoint, sizeof(checkpoint), &policy) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(UmiAiTrainingGradientPolicyVerifyFile(
        path, checkpoint, sizeof(checkpoint), &policy) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingGradientPolicyVerifyFile(
        path, checkpoint, sizeof(checkpoint), &changed) == UMI_STATUS_PARSE_ERROR);
    file = fopen(path, "ab");
    CHECK(file != NULL);
    CHECK(fputc('!', file) != EOF);
    CHECK(fclose(file) == 0);
    CHECK(UmiAiTrainingGradientPolicyVerifyFile(
        path, checkpoint, sizeof(checkpoint), &policy) == UMI_STATUS_PARSE_ERROR);
    CHECK(remove(path) == 0);
    puts("clipping policy companion file no-clobber and rejection: PASS");
    return 0;
}
