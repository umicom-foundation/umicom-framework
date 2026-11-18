/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/cvm_source_check/main.c
 *
 * PURPOSE:
 *   Demonstrate native source diagnostics associated with a document revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/cvm_source_check.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    const char source[]="int main(void){int unused=7;return 42;}";
    const uint64_t document_revision=41U;
    UmiCvmSourceCheckResult result;
    if(umi_cvm_check_source(source,strlen(source),document_revision,&result,sizeof result)!=UMI_STATUS_OK){
        fprintf(stderr,"Source check failed: %s\n",result.diagnostic.message);
        return 1;
    }
    if(!umi_cvm_source_check_matches(&result,sizeof result,document_revision))return 1;
    printf("Revision %" PRIu64 ": %zu warnings; %zu instructions checked, not executed.\n",
        result.source_revision,result.report.warning_count,result.report.instruction_count);
    for(size_t i=0;i<result.report.warning_count;++i){
        const UmcSourceWarning *warning=&result.report.warnings[i];
        printf("%zu:%zu: local '%.*s' is never read\n",warning->line,warning->column,
            (int)warning->source_length,source+warning->source_offset);
    }
    /* An editor increments its revision when text changes. It must then reject
     * this old result even before a replacement check has completed. */
    if(umi_cvm_source_check_matches(&result,sizeof result,document_revision+1U))return 1;
    puts("A newer source revision requires a new check.");
    return ferror(stdout)?1:0;
}
