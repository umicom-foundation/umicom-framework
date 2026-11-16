/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/model_formats/test_gguf_file.c
 *
 * PURPOSE:
 *   Inspect real temporary GGUF files through the shared checked reader without launching a model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "gguf_fixture.h"
#include "../build_log/fixture.h"
#define MODE(name) (strcmp(mode,(name))==0)
int main(int argc,char **argv)
{
/* Make independent fixture statements explicit without weakening the existing checks. The former implementation is retained for owner review. */
#if 0
    if(argc!=2)return 2;const char *mode=argv[1];
#endif
    if(argc!=2) { return 2; }
    const char *mode=argv[1];
    char root[UMI_PATH_CAPACITY],path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);FixturePath(path,root,MODE("unicode")?"model-\xc3\xa9.gguf":"model.gguf");
    GgufFixture *f=malloc(sizeof(*f));GGUF_CHECK(f!=NULL);fixture_model(f);
    if(MODE("auxiliary")){fixture_text(f,"general.type","adapter");}
    unsigned char payload[32]={0};fixture_bytes(f,payload,sizeof(payload));
    UmiOutputFile *file=NULL;GGUF_CHECK(UmiOutputFileCreate(path,&file)==UMI_STATUS_OK);
    GGUF_CHECK(UmiOutputFileWrite(file,f->bytes,f->size)==UMI_STATUS_OK);
    GGUF_CHECK(UmiOutputFileClose(file)==UMI_STATUS_OK);UmiOutputFileDestroy(file);
    UmiGgufFileInspection output;memset(&output,0xa5,sizeof(output));
    unsigned char before[sizeof(output)];memcpy(before,&output,sizeof(output));
    UmiStatus wanted=UMI_STATUS_OK;const char *selected=path;
    if(MODE("directory")){selected=root;wanted=UMI_STATUS_IO_ERROR;}
    else if(MODE("relative")){selected="model.gguf";wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(MODE("missing")){FixturePath(path,root,"absent.gguf");wanted=UMI_STATUS_NOT_FOUND;}
    else if(!MODE("valid") && !MODE("unicode") && !MODE("auxiliary") && !MODE("expected")){free(f);return 2;}
    GGUF_CHECK(UmiGgufFileInspect(selected,NULL,NULL,&output)==wanted);
    if(wanted!=UMI_STATUS_OK)GGUF_CHECK(memcmp(before,&output,sizeof(output))==0);
    else {
        GGUF_CHECK(output.file.bytes==f->size && strcmp(output.metadata.architecture,"llama")==0);
        GGUF_CHECK(output.metadata.role==(MODE("auxiliary")?UMI_GGUF_ROLE_AUXILIARY:UMI_GGUF_ROLE_MODEL_CANDIDATE));
        if(MODE("expected"))GGUF_CHECK(UmiGgufFileInspect(path,&output.file,NULL,&output)==UMI_STATUS_OK);
    }
    free(f);return 0;
}
