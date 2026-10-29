/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_environment/main.c
 *
 * PURPOSE:
 *   Compare installed header declarations with a real independent C23 build.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include <stdio.h>
#ifndef UMICOM_REQUIRED_ABI
#error "Configure this consumer to declare its required Framework ABI."
#endif
#ifndef UMICOM_PACKAGE_POINTER_BYTES
#error "Configure this consumer with the installed package metadata."
#endif
/* These assertions compare compiler facts and installed headers with package
 * declarations. They do not prove layout compatibility for every public type. */
static_assert(UMICOM_FRAMEWORK_ABI_VERSION == UMICOM_REQUIRED_ABI,
    "Installed header ABI differs from the consumer requirement");
static_assert(sizeof(void *) == UMICOM_PACKAGE_POINTER_BYTES,
    "Installed package pointer size differs from the consumer compiler");
int main(void)
{
    /* Calling a real library symbol distinguishes a linked consumer from a
     * program which only prints values copied from a header. */
    const char *status = umi_status_text(UMI_STATUS_OK);
    if (status == NULL || status[0] == '\0') {
        fputs("The linked base library returned no status text.\n", stderr);
        return 1;
    }
    printf("Framework headers: %s\n", UMICOM_FRAMEWORK_VERSION_STRING);
    printf("Required ABI: %u\n", (unsigned)UMICOM_REQUIRED_ABI);
    printf("Consumer pointer bytes: %zu\n", sizeof(void *));
    printf("Linked base status: %s\n", status);
    return 0;
}
