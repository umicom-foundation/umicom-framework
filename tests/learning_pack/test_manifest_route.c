/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_manifest_route.c
 *
 * PURPOSE:
 *   Verify course manifest provenance survives owned copies and rejects invalid reads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "manifest_route_fixture.h"
#include "umicom/teacher/learning_reader.h"
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); result=1; goto cleanup; } } while(0)
int main(int argc, char **argv)
{
    if(argc != 3) return 2;
    int result=0;
    UmiVersion host={0U,9U,0U};
    UmiLearningPack *pack=NULL,*copy=NULL,*snapshot=NULL;
    UmiLearningShelf *shelf=NULL;
    UmiLearningReader *reader=NULL;
    unsigned char *bytes=NULL;
    size_t size=0U;
    char route[UMI_TEACHER_PATH_CAPACITY]="first-course.json";
    char output[UMI_TEACHER_PATH_CAPACITY]="unchanged";
    CHECK(RouteFixture(argv[2],route,"route.first",1U)==UMI_STATUS_OK);
    CHECK(UmiLearningPackLoad(argv[2],route,host,&pack)==UMI_STATUS_OK);
    if(strcmp(argv[1],"captured")==0) {
        route[0]='X';
        CHECK(UmiLearningPackCopyManifestPath(pack,output,sizeof(output))==UMI_STATUS_OK);
        CHECK(strcmp(output,"first-course.json")==0);
    } else if(strcmp(argv[1],"copy-lifetimes")==0) {
        CHECK(UmiLearningPackClone(pack,&copy)==UMI_STATUS_OK);
        UmiLearningPackDestroy(pack); pack=NULL;
        CHECK(UmiLearningShelfCreate(&shelf)==UMI_STATUS_OK);
        CHECK(UmiLearningShelfAdopt(shelf,&copy)==UMI_STATUS_OK && copy==NULL);
        CHECK(UmiLearningShelfCopyPack(shelf,"route.first",&copy)==UMI_STATUS_OK);
        UmiLearningShelfDestroy(shelf); shelf=NULL;
        CHECK(UmiLearningReaderCreate(&copy,4096U,&reader)==UMI_STATUS_OK && copy==NULL);
        CHECK(UmiLearningReaderOpen(reader,"lesson")==UMI_STATUS_OK);
        CHECK(UmiLearningReaderCopyPack(reader,&snapshot)==UMI_STATUS_OK);
        UmiLearningReaderDestroy(reader); reader=NULL;
        CHECK(UmiLearningPackCopyManifestPath(snapshot,output,sizeof(output))==UMI_STATUS_OK);
        CHECK(strcmp(output,route)==0);
    } else if(strcmp(argv[1],"capacity")==0) {
        CHECK(UmiLearningPackCopyManifestPath(pack,output,strlen(route))==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output,"unchanged")==0);
        CHECK(UmiLearningPackCopyManifestPath(pack,output,strlen(route)+1U)==UMI_STATUS_OK);
        CHECK(strcmp(output,route)==0);
    } else if(strcmp(argv[1],"arguments")==0) {
        CHECK(UmiLearningPackCopyManifestPath(NULL,output,sizeof(output))==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPackCopyManifestPath(pack,NULL,10U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPackCopyManifestPath(pack,output,0U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output,"unchanged")==0);
    } else if(strcmp(argv[1],"metadata-only")==0) {
        CHECK(UmiRootedFileRead(argv[2],route,4096U,&bytes,&size)==UMI_STATUS_OK);
        CHECK(UmiLearningPackCreate(bytes,size,argv[2],host,&copy)==UMI_STATUS_OK);
        CHECK(UmiLearningPackCopyManifestPath(copy,output,sizeof(output))==UMI_STATUS_NOT_FOUND);
        CHECK(strcmp(output,"unchanged")==0);
    } else if(strcmp(argv[1],"invalid-route")==0) {
        CHECK(UmiLearningPackLoad(argv[2],"../first-course.json",host,&copy)!=UMI_STATUS_OK && copy==NULL);
        CHECK(UmiLearningPackLoad(argv[2],NULL,host,&copy)==UMI_STATUS_INVALID_ARGUMENT && copy==NULL);
        memset(route,'a',sizeof(route)-1U); route[sizeof(route)-1U]='\0';
        CHECK(UmiLearningPackLoad(argv[2],route,host,&copy)!=UMI_STATUS_OK && copy==NULL);
    } else if(strcmp(argv[1],"two-courses")==0) {
        CHECK(RouteFixture(argv[2],"second-course.json","route.second",1U)==UMI_STATUS_OK);
        CHECK(UmiLearningPackLoad(argv[2],"second-course.json",host,&copy)==UMI_STATUS_OK);
        CHECK(UmiLearningShelfCreate(&shelf)==UMI_STATUS_OK);
        CHECK(UmiLearningShelfAdopt(shelf,&pack)==UMI_STATUS_OK);
        CHECK(UmiLearningShelfAdopt(shelf,&copy)==UMI_STATUS_OK);
        CHECK(UmiLearningShelfCount(shelf)==2U);
        CHECK(UmiLearningShelfCopyPack(shelf,"route.second",&snapshot)==UMI_STATUS_OK);
        CHECK(UmiLearningPackCopyManifestPath(snapshot,output,sizeof(output))==UMI_STATUS_OK);
        CHECK(strcmp(output,"second-course.json")==0);
    } else CHECK(0);
cleanup:
    UmiLearningPackFreeBytes(bytes);
    UmiLearningReaderDestroy(reader);
    UmiLearningShelfDestroy(shelf);
    UmiLearningPackDestroy(snapshot);
    UmiLearningPackDestroy(copy);
    UmiLearningPackDestroy(pack);
    return result;
}
