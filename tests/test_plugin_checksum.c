/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_plugin_checksum.c
 *
 * PURPOSE:
 *   Verify deterministic plug-in file checksums and text formatting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/plugin/checksum.h"
int main(void) {
    const char *path = "umicom-plugin-checksum.tmp"; FILE *f = fopen(path,"wb"); uint64_t checksum,size; char text[17];
    assert(f != NULL); assert(fwrite("abc",1U,3U,f) == 3U); assert(fclose(f) == 0);
    assert(umi_plugin_checksum_file(path,&checksum,&size) == UMI_STATUS_OK && size == 3U);
    assert(umi_plugin_checksum_text(checksum,text,sizeof(text)) == UMI_STATUS_OK && strlen(text) == 16U);
    (void)remove(path); return 0;
}
