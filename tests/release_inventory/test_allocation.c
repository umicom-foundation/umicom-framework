/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_inventory/test_allocation.c
 * PURPOSE:
 *   Refuse each parser allocation without publishing a partial inventory.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Refuse each parser allocation without publishing a partial inventory. */
#include "umicom/distribution/runtime/inventory.h"
#include <stdlib.h>
#include <string.h>
void *__real_malloc(size_t size);
void *__real_calloc(size_t count, size_t size);
void *__wrap_malloc(size_t size);
void *__wrap_calloc(size_t count, size_t size);
static size_t remaining;
static bool failing;
void *__wrap_malloc(size_t size)
{ if (failing && remaining-- == 0U) return NULL; return __real_malloc(size); }
void *__wrap_calloc(size_t count, size_t size)
{ if (failing && remaining-- == 0U) return NULL; return __real_calloc(count, size); }
int main(void)
{
    const char text[] = "UMICOM-RELEASE-INVENTORY\t1\ncontext\tcmake\t0123456789abcdef0123456789abcdef\t2f\t2f\t61\n";
    for (size_t i = 0U; i < 3U; ++i) {
        UmiReleaseInventory *value = NULL;
        remaining = i; failing = true;
        UmiStatus status = UmiReleaseInventoryParse(text, strlen(text), &value);
        failing = false;
        if (status != UMI_STATUS_OUT_OF_MEMORY || value != NULL) { UmiReleaseInventoryDestroy(value); return 1; }
    }
    UmiReleaseInventory *value = NULL;
    if (UmiReleaseInventoryParse(text, strlen(text), &value) != UMI_STATUS_OK) return 1;
    UmiReleaseInventoryDestroy(value);
    return 0;
}
