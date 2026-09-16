/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/bounded_strings/main.c
 *
 * PURPOSE:
 *   Bounded text and null termination.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* sourceCapacity is the actual readable extent; source/destination must not overlap. */
static bool CopyField(char *destination, size_t destinationCapacity,
                      const char *source, size_t sourceCapacity)
{
    if (destination==NULL || source==NULL || destinationCapacity==0U) return false;
    const char *end=memchr(source,'\0',sourceCapacity);
    if (end==NULL) return false;
    size_t length=(size_t)(end-source);
    if (length>=destinationCapacity) return false;
    memcpy(destination,source,length+1U); return true;
}
int main(void)
{
    char output[5]="keep";
    const char broken[4]={'x','x','x','x'};
    if (CopyField(output,sizeof(output),broken,sizeof(broken)) || strcmp(output,"keep")!=0)
        return EXIT_FAILURE;
    if (CopyField(output,sizeof(output),"large",sizeof("large"))) return EXIT_FAILURE;
    if (!CopyField(output,sizeof(output),"four",sizeof("four")) || strcmp(output,"four")!=0)
        return EXIT_FAILURE;
    if (!CopyField(output,sizeof(output),"",sizeof("")) || output[0]!='\0') return EXIT_FAILURE;
    puts("malformed and oversized fields rejected; exact fit accepted");
    puts("PASS: bounded_strings");
    return EXIT_SUCCESS;
}
