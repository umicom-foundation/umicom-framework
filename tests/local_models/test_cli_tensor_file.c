/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_cli_tensor_file.c
 *
 * PURPOSE:
 *   Exercise the linked native model command against isolated synthetic tensor files and malformed inputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../build_log/fixture.h"
#include "../model_formats/safetensors_fixture.h"
#include "umicom/local_models/manager.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);
    bool valid=strcmp(argv[1],"valid")==0,malformed=strcmp(argv[1],"malformed")==0;
    bool unicode=strcmp(argv[1],"unicode")==0,relative=strcmp(argv[1],"relative")==0;
    CHECK(valid || malformed || unicode || relative);
    char root[UMI_PATH_CAPACITY],path[UMI_PATH_CAPACITY];FixtureDirectory(root);
    FixturePath(path,root,unicode?"caf\xc3\xa9.safetensors":"weights.safetensors");
    size_t size;unsigned char *bytes=tensor_fixture_prefix(tensor_fixture_json,24U,&size);CHECK(bytes!=NULL);
    if(malformed) tensor_fixture_length(bytes,1U);
    UmiOutputFile *file=NULL;CHECK(UmiOutputFileCreate(path,&file)==UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file,bytes,size)==UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file)==UMI_STATUS_OK);UmiOutputFileDestroy(file);free(bytes);
    char *command[]={"inspect-safetensors",relative?"relative.safetensors":path};
    UmiModelSession *session=NULL;
    CHECK(UmiModelsCommandWithSession(2,command,&session)==(relative?2:malformed?1:0));
    CHECK(session==NULL && !UmiModelsCommandHasRetainedSession());
    return 0;
}
