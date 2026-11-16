/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/ollama_pull_command.c
 *
 * PURPOSE:
 *   Provide one read-only progress-file command for Media, native CLI and SDK hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/ollama_pull_file.h"
#include <stdio.h>
#include <string.h>

int UmiOllamaPullFileCommand(int argc,char **argv)
{
    if(argc!=2 || argv==NULL || argv[0]==NULL || argv[1]==NULL || strcmp(argv[0],"inspect-pull")!=0){
        fputs("Use: models inspect-pull ABSOLUTE_NDJSON | models inspect-pull --help\n",stderr);return 2;
    }
    if(strcmp(argv[1],"--help")==0){
        fputs("Inspect one captured Ollama pull transcript; no endpoint or model operation.\n"
            "Outputs file SHA-256, counts and reported outcome; excludes provider messages.\n"
            "Exit 0: reported success; 3: reported error; 2: usage; 1: inspection/output failure.\n",stdout);
        return fflush(stdout)==0 && !ferror(stdout)?0:1;
    }
    UmiOllamaPullFileReport report={0};UmiStatus status=UmiOllamaPullFileInspect(argv[1],NULL,&report);
    char json[UMI_OLLAMA_PULL_FILE_JSON_CAPACITY];size_t length=0U;
    if(status==UMI_STATUS_OK)status=UmiOllamaPullFileJson(&report,json,sizeof json,&length);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Umicom model-progress inspection: %s (%d).\n",umi_status_text(status),(int)status);return 1;}
    if(fwrite(json,1U,length,stdout)!=length || fputc('\n',stdout)==EOF || fflush(stdout)!=0 || ferror(stdout))return 1;
    return report.provider_outcome==UMI_OLLAMA_PULL_ERROR?3:0;
}
