/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/resource_cleanup/main.c
 *
 * PURPOSE:
 *   Files, errors and one cleanup path.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    /* Run in a private practice/build directory. Exclusive creation preserves
     * any pre-existing file. Do not use the privileged Windows tmpfile root. */
    const char *path="umicom-learning-resource-cleanup.tmp";
    FILE *stream=fopen(path,"w+bx");
    if (stream==NULL) {
        fputs("Cannot create scratch file: use a writable practice directory; preserve an existing file.\n",stderr);
        return EXIT_FAILURE;
    }
    int result=EXIT_FAILURE;
    const char message[]="learning\n";
    char recovered[32]={0};
    if (fwrite(message,1U,sizeof(message)-1U,stream)!=sizeof(message)-1U) goto cleanup;
    if (fflush(stream)!=0 || fseek(stream,0L,SEEK_SET)!=0) goto cleanup;
    if (fgets(recovered,sizeof(recovered),stream)==NULL) goto cleanup;
    if (strcmp(recovered,message)!=0) goto cleanup;
    result=EXIT_SUCCESS;
cleanup:
    if (fclose(stream)!=0) result=EXIT_FAILURE;
    /* We reach this line only after creating the file ourselves. */
    if (remove(path)!=0) result=EXIT_FAILURE;
    if (result!=EXIT_SUCCESS) return result;
    puts("new scratch file written, read, closed and removed");
    puts("PASS: resource_cleanup");
    return EXIT_SUCCESS;
}
