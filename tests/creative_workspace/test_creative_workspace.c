/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_creative_workspace.c
 * PURPOSE: Exercise real creative services, canonical animation and Data Server failure paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/workspace.h"
#include "umicom/creative_workspace/scene.h"
#include "umicom/creative_workspace/export.h"
#include "internal.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
/* Checks remain active in Release. The runner always releases the fixture,
 * including after a failed check; no external application is launched. */
#define CHECK(condition) do { if(!(condition)) { \
    fprintf(stderr,"Line %d: %s\n",__LINE__,#condition); return 1; } } while(0)
#define OK(call) CHECK((call)==UMI_STATUS_OK)
typedef struct Fixture {
    UmiCreativeProject *project,*other;
    UmiCreativeElement element;
    UmiCreativeFrame frame;
    UmiCreativePlayState play;
    UmiCreativeExport exported;
    UmiCreativeWorkspace *workspace,*second,*third;
    UmiDataServer *server,*secondServer;
    UmiMediaAnimationTrack *track;
} Fixture;
static Fixture f;
static UmiStatus Add(const char *id,int32_t x,int32_t y,int32_t width,int32_t height)
{
    UmiStatus status=UmiCreativeElementInit(&f.element,id,id,UMI_CREATIVE_RECTANGLE,x,y,width,height,0x28668CU);
    return status==UMI_STATUS_OK?UmiCreativeProjectPutElement(f.project,&f.element,false):status;
}
static UmiStatus Start(bool sqlite)
{
    UmiStatus status;
    if(sqlite){(void)remove("fixture.sqlite3");status=umi_data_server_create_sqlite("fixture.sqlite3",&f.server);}
    else status=umi_data_server_create_memory(&f.server);
    if(status==UMI_STATUS_OK)status=UmiCreativeWorkspaceCreate(f.server,f.project,&f.workspace);
    return status==UMI_STATUS_OK?UmiCreativeWorkspaceCopy(f.workspace,f.project):status;
}
static UmiStatus Save(void)
{
    UmiStatus status=UmiCreativeWorkspaceReplace(f.workspace,f.project,f.project->revision);
    return status==UMI_STATUS_OK?UmiCreativeWorkspaceCopy(f.workspace,f.project):status;
}
static uint32_t Read32(const unsigned char *p)
{
    return (uint32_t)p[0]|((uint32_t)p[1]<<8U)|((uint32_t)p[2]<<16U)|((uint32_t)p[3]<<24U);
}
static bool AbsoluteOutput(char *path,size_t capacity,const char *name)
{
    char cwd[2048];
#ifdef _WIN32
    if(_getcwd(cwd,sizeof(cwd))==NULL)return false;
#else
    if(getcwd(cwd,sizeof(cwd))==NULL)return false;
#endif
    int count=snprintf(path,capacity,"%s/%s",cwd,name);
    return count>0&&(size_t)count<capacity;
}

/* project.initialise: exercise the public behaviour named by this regression. */
static int Case0(void)
{
CHECK(f.project->elementCount==0U && f.project->revision==0U); OK(UmiCreativeProjectValidate(f.project)); CHECK(strcmp(UmiCreativeUnitText(UMI_CREATIVE_PIXELS),"pixels")==0);
return 0;
}

/* project.invalid_identity: exercise the public behaviour named by this regression. */
static int Case1(void)
{
const char *ids[]={"", "../other", "contains space", "/absolute", "a:b", "-bad"}; for(size_t i=0;i<sizeof(ids)/sizeof(ids[0]);++i) CHECK(UmiCreativeProjectInit(f.other,ids[i],&f.project->settings)!=UMI_STATUS_OK);
return 0;
}

/* project.utf8_labels: exercise the public behaviour named by this regression. */
static int Case2(void)
{
OK(UmiCreativeElementInit(&f.element,"label","Café — أدوات",UMI_CREATIVE_TEXT,0,0,200,40,0x123456U)); OK(UmiCreativeProjectPutElement(f.project,&f.element,false)); CHECK(UmiCreativeElementInit(&f.element,"bad","bad\nlabel",UMI_CREATIVE_TEXT,0,0,20,20,0U)!=UMI_STATUS_OK);
return 0;
}

/* project.malformed_utf8: exercise the public behaviour named by this regression. */
static int Case3(void)
{
const char *texts[]={"\xc0\xaf","\xed\xa0\x80","\xf4\x90\x80\x80","\x80","\xe2\x82","\xef\xbf\xbf"}; for(size_t i=0;i<sizeof(texts)/sizeof(texts[0]);++i) CHECK(UmiCreativeElementInit(&f.element,"bad",texts[i],UMI_CREATIVE_TEXT,0,0,20,20,0U)!=UMI_STATUS_OK);
return 0;
}

/* project.settings_boundaries: exercise the public behaviour named by this regression. */
static int Case4(void)
{
UmiCreativeSettings settings=f.project->settings; settings.width=0; CHECK(UmiCreativeProjectConfigure(f.project,&settings)!=UMI_STATUS_OK); settings=f.project->settings;settings.tempoBpm=19U;CHECK(UmiCreativeProjectConfigure(f.project,&settings)!=UMI_STATUS_OK);settings=f.project->settings;settings.durationMs=30001U;CHECK(UmiCreativeProjectConfigure(f.project,&settings)!=UMI_STATUS_OK); CHECK(f.project->settings.width==960);
return 0;
}

/* project.add_replace_remove: exercise the public behaviour named by this regression. */
static int Case5(void)
{
OK(Add("box",10,20,100,80)); CHECK(UmiCreativeProjectPutElement(f.project,&f.element,false)==UMI_STATUS_ALREADY_EXISTS); f.element.x=40;OK(UmiCreativeProjectPutElement(f.project,&f.element,true)); CHECK(f.project->elements[0].x==40);OK(UmiCreativeProjectRemoveElement(f.project,"box"));CHECK(f.project->elementCount==0U);CHECK(UmiCreativeProjectRemoveElement(f.project,"box")==UMI_STATUS_NOT_FOUND);
return 0;
}

/* project.failed_append_unchanged: exercise the public behaviour named by this regression. */
static int Case6(void)
{
memset(&f.project->elements[0],0x5A,sizeof(f.project->elements[0]));*f.other=*f.project;OK(UmiCreativeElementInit(&f.element,"oversize","Oversize",UMI_CREATIVE_RECTANGLE,0,0,961,30,0U));CHECK(UmiCreativeProjectPutElement(f.project,&f.element,false)!=UMI_STATUS_OK);CHECK(memcmp(f.project,f.other,sizeof(*f.project))==0);
return 0;
}

/* project.failed_replace_unchanged: exercise the public behaviour named by this regression. */
static int Case7(void)
{
OK(Add("box",10,20,100,80));*f.other=*f.project;f.element.x=INT32_MAX;CHECK(UmiCreativeProjectPutElement(f.project,&f.element,true)!=UMI_STATUS_OK);CHECK(memcmp(f.project,f.other,sizeof(*f.project))==0);
return 0;
}

/* project.element_capacity: exercise the public behaviour named by this regression. */
static int Case8(void)
{
for(size_t i=0;i<UMI_CREATIVE_MAX_ELEMENTS;++i){char id[32];(void)snprintf(id,sizeof(id),"item-%zu",i);OK(Add(id,0,0,10,10));}OK(UmiCreativeElementInit(&f.element,"extra","Extra",UMI_CREATIVE_RECTANGLE,0,0,10,10,0U));CHECK(UmiCreativeProjectPutElement(f.project,&f.element,false)==UMI_STATUS_CAPACITY_EXCEEDED);
return 0;
}

/* project.layer_order: exercise the public behaviour named by this regression. */
static int Case9(void)
{
OK(Add("a",0,0,50,50));OK(Add("b",0,0,50,50));OK(Add("c",0,0,50,50));OK(UmiCreativeProjectMoveLayer(f.project,"a",2U));CHECK(strcmp(f.project->elements[2].id,"a")==0);OK(UmiCreativeProjectMoveLayer(f.project,"a",0U));CHECK(strcmp(f.project->elements[0].id,"a")==0);CHECK(UmiCreativeProjectMoveLayer(f.project,"a",3U)!=UMI_STATUS_OK);
return 0;
}

/* project.unit_change_guard: exercise the public behaviour named by this regression. */
static int Case10(void)
{
OK(Add("a",0,0,20,20));UmiCreativeSettings settings=f.project->settings;settings.unit=UMI_CREATIVE_MILLIMETRES;CHECK(UmiCreativeProjectConfigure(f.project,&settings)==UMI_STATUS_INVALID_STATE);
return 0;
}

/* project.keys_start_at_zero: exercise the public behaviour named by this regression. */
static int Case11(void)
{
OK(Add("a",0,0,50,50));UmiCreativeKey key={1000U,10,0,UMI_MEDIA_ANIMATION_LINEAR};CHECK(UmiCreativeProjectPutKey(f.project,"a",&key)!=UMI_STATUS_OK);CHECK(f.project->elements[0].keyCount==0U);key.timeMs=0U;OK(UmiCreativeProjectPutKey(f.project,"a",&key));key.timeMs=1000U;OK(UmiCreativeProjectPutKey(f.project,"a",&key));CHECK(UmiCreativeProjectRemoveKey(f.project,"a",0U)==UMI_STATUS_INVALID_STATE);
return 0;
}

/* project.key_sort_update: exercise the public behaviour named by this regression. */
static int Case12(void)
{
OK(Add("a",20,20,50,50));UmiCreativeKey key={0U,0,0,UMI_MEDIA_ANIMATION_LINEAR};OK(UmiCreativeProjectPutKey(f.project,"a",&key));key.timeMs=2000U;key.offsetX=20;OK(UmiCreativeProjectPutKey(f.project,"a",&key));key.timeMs=1000U;key.offsetX=10;OK(UmiCreativeProjectPutKey(f.project,"a",&key));CHECK(f.project->elements[0].keys[1].timeMs==1000U);key.offsetX=30;OK(UmiCreativeProjectPutKey(f.project,"a",&key));CHECK(f.project->elements[0].keyCount==3U);CHECK(f.project->elements[0].keys[1].offsetX==30);
return 0;
}

/* project.key_bounds: exercise the public behaviour named by this regression. */
static int Case13(void)
{
OK(Add("a",20,20,50,50));UmiCreativeKey key={0U,-21,0,UMI_MEDIA_ANIMATION_LINEAR};CHECK(UmiCreativeProjectPutKey(f.project,"a",&key)!=UMI_STATUS_OK);key.offsetX=0;OK(UmiCreativeProjectPutKey(f.project,"a",&key));key.timeMs=8001U;CHECK(UmiCreativeProjectPutKey(f.project,"a",&key)!=UMI_STATUS_OK);key.timeMs=200U;key.interpolation=(UmiMediaAnimationInterpolation)99;CHECK(UmiCreativeProjectPutKey(f.project,"a",&key)!=UMI_STATUS_OK);
return 0;
}

/* project.key_capacity: exercise the public behaviour named by this regression. */
static int Case14(void)
{
OK(Add("a",0,0,50,50));UmiCreativeKey key={0};for(size_t i=0;i<8U;++i){key.timeMs=(uint32_t)i*100U;OK(UmiCreativeProjectPutKey(f.project,"a",&key));}key.timeMs=800U;CHECK(UmiCreativeProjectPutKey(f.project,"a",&key)==UMI_STATUS_CAPACITY_EXCEEDED);key.timeMs=200U;key.offsetX=50;OK(UmiCreativeProjectPutKey(f.project,"a",&key));
return 0;
}

/* project.note_timing: exercise the public behaviour named by this regression. */
static int Case15(void)
{
UmiCreativeNote note={.id="a4",.startTick=7200U,.durationTicks=480U,.pitch=69U,.velocity=100U};OK(UmiCreativeProjectPutNote(f.project,&note,false));note.durationTicks=481U;CHECK(UmiCreativeProjectPutNote(f.project,&note,true)!=UMI_STATUS_OK);CHECK(f.project->notes[0].durationTicks==480U);
return 0;
}

/* project.note_invalid: exercise the public behaviour named by this regression. */
static int Case16(void)
{
UmiCreativeNote note={.id="a",.durationTicks=480U,.pitch=69U,.velocity=0U};CHECK(UmiCreativeProjectPutNote(f.project,&note,false)!=UMI_STATUS_OK);note.velocity=128U;CHECK(UmiCreativeProjectPutNote(f.project,&note,false)!=UMI_STATUS_OK);note.velocity=80U;note.pitch=128U;CHECK(UmiCreativeProjectPutNote(f.project,&note,false)!=UMI_STATUS_OK);note.pitch=60U;note.startTick=UINT32_MAX;CHECK(UmiCreativeProjectPutNote(f.project,&note,false)!=UMI_STATUS_OK);
return 0;
}

/* project.note_capacity: exercise the public behaviour named by this regression. */
static int Case17(void)
{
UmiCreativeNote note={.durationTicks=1U,.pitch=60U,.velocity=80U};for(size_t i=0;i<64U;++i){(void)snprintf(note.id,sizeof(note.id),"note-%zu",i);OK(UmiCreativeProjectPutNote(f.project,&note,false));}(void)snprintf(note.id,sizeof(note.id),"extra");CHECK(UmiCreativeProjectPutNote(f.project,&note,false)==UMI_STATUS_CAPACITY_EXCEEDED);OK(UmiCreativeProjectRemoveNote(f.project,"note-10"));CHECK(f.project->noteCount==63U);
return 0;
}

/* project.failed_note_append_unchanged: exercise the public behaviour named by this regression. */
static int Case18(void)
{
memset(&f.project->notes[0],0x5A,sizeof(f.project->notes[0]));*f.other=*f.project;UmiCreativeNote note={.id="bad",.durationTicks=1U};CHECK(UmiCreativeProjectPutNote(f.project,&note,false)!=UMI_STATUS_OK);CHECK(memcmp(f.project,f.other,sizeof(*f.project))==0);
return 0;
}

/* project.starters: exercise the public behaviour named by this regression. */
static int Case19(void)
{
const char *profiles[]={"cad","kitchen","media","music","games","web-studio","mobile-studio"};for(size_t i=0;i<7U;++i){*f.other=*f.project;OK(UmiCreativeProjectStarter(f.other,profiles[i]));OK(UmiCreativeProjectValidate(f.other));CHECK(f.other->elementCount>0U);CHECK(UmiCreativeProjectStarter(f.other,profiles[i])==UMI_STATUS_INVALID_STATE);}CHECK(UmiCreativeProjectStarter(f.project,"unknown")==UMI_STATUS_INVALID_ARGUMENT);CHECK(f.project->elementCount==0U);
return 0;
}

/* workspace.create_open: exercise the public behaviour named by this regression. */
static int Case20(void)
{
OK(Start(false));CHECK(f.project->revision==1U);OK(UmiCreativeWorkspaceOpen(f.server,"practice",&f.second));OK(UmiCreativeWorkspaceCopy(f.second,f.other));CHECK(f.other->revision==1U);CHECK(UmiCreativeWorkspaceCreate(f.server,f.project,&f.third)==UMI_STATUS_ALREADY_EXISTS);
return 0;
}

/* workspace.undo_redo: exercise the public behaviour named by this regression. */
static int Case21(void)
{
OK(Start(false));OK(Add("a",20,20,50,50));OK(Save());CHECK(f.project->revision==2U);OK(UmiCreativeWorkspaceUndo(f.workspace,2U));OK(UmiCreativeWorkspaceCopy(f.workspace,f.project));CHECK(f.project->revision==3U&&f.project->elementCount==0U);OK(UmiCreativeWorkspaceRedo(f.workspace,3U));OK(UmiCreativeWorkspaceCopy(f.workspace,f.project));CHECK(f.project->revision==4U&&f.project->elementCount==1U);
return 0;
}

/* workspace.undo_branch: exercise the public behaviour named by this regression. */
static int Case22(void)
{
OK(Start(false));OK(Add("a",20,20,50,50));OK(Save());OK(UmiCreativeWorkspaceUndo(f.workspace,f.project->revision));OK(UmiCreativeWorkspaceCopy(f.workspace,f.project));OK(Add("b",30,20,50,50));OK(Save());CHECK(UmiCreativeWorkspaceRedo(f.workspace,f.project->revision)==UMI_STATUS_NOT_FOUND);CHECK(strcmp(f.project->elements[0].id,"b")==0);
return 0;
}

/* workspace.history_capacity: exercise the public behaviour named by this regression. */
static int Case23(void)
{
OK(Start(false));for(unsigned i=0;i<20U;++i){f.project->settings.tempoBpm=100U+i;OK(Save());}UmiCreativeHistory history;OK(UmiCreativeWorkspaceHistory(f.workspace,&history));CHECK(history.undoCount==16U);for(unsigned i=0;i<16U;++i){OK(UmiCreativeWorkspaceUndo(f.workspace,f.project->revision));OK(UmiCreativeWorkspaceCopy(f.workspace,f.project));}CHECK(UmiCreativeWorkspaceUndo(f.workspace,f.project->revision)==UMI_STATUS_NOT_FOUND);CHECK(f.project->settings.tempoBpm==103U);
return 0;
}

/* workspace.noop_padding: exercise the public behaviour named by this regression. */
static int Case24(void)
{
OK(Start(false));memset(&f.project->elements[10],0xA5,sizeof(f.project->elements[10]));OK(Save());CHECK(f.project->revision==1U);UmiCreativeHistory history;OK(UmiCreativeWorkspaceHistory(f.workspace,&history));CHECK(history.undoCount==0U);
return 0;
}

/* workspace.expected_revision: exercise the public behaviour named by this regression. */
static int Case25(void)
{
OK(Start(false));CHECK(UmiCreativeWorkspaceReplace(f.workspace,f.project,0U)==UMI_STATUS_BUSY);f.project->revision=2U;CHECK(UmiCreativeWorkspaceReplace(f.workspace,f.project,1U)==UMI_STATUS_BUSY);
return 0;
}

/* workspace.stale_handle: exercise the public behaviour named by this regression. */
static int Case26(void)
{
OK(Start(false));OK(UmiCreativeWorkspaceOpen(f.server,"practice",&f.second));OK(Add("a",0,0,20,20));OK(Save());OK(UmiCreativeWorkspaceCopy(f.second,f.other));CHECK(UmiCreativeWorkspaceReplace(f.second,f.other,1U)==UMI_STATUS_BUSY);OK(UmiCreativeWorkspaceReload(f.second));OK(UmiCreativeWorkspaceCopy(f.second,f.other));CHECK(f.other->revision==2U);
return 0;
}

/* workspace.reload_clears_history: exercise the public behaviour named by this regression. */
static int Case27(void)
{
OK(Start(false));OK(Add("a",0,0,20,20));OK(Save());OK(UmiCreativeWorkspaceReload(f.workspace));UmiCreativeHistory history;OK(UmiCreativeWorkspaceHistory(f.workspace,&history));CHECK(history.undoCount==0U&&history.redoCount==0U);
return 0;
}

/* workspace.foreign_transaction: exercise the public behaviour named by this regression. */
static int Case28(void)
{
OK(Start(false));OK(umi_data_server_begin(f.server));OK(umi_data_server_set(f.server,"foreign","preserve"));CHECK(UmiCreativeWorkspaceReload(f.workspace)!=UMI_STATUS_OK);CHECK(umi_data_server_in_transaction(f.server));char value[32];OK(umi_data_server_get(f.server,"foreign",value,sizeof(value)));CHECK(strcmp(value,"preserve")==0);OK(umi_data_server_rollback(f.server));
return 0;
}

/* workspace.missing_child: exercise the public behaviour named by this regression. */
static int Case29(void)
{
OK(Start(false));OK(Add("a",0,0,20,20));OK(Save());OK(umi_data_server_delete(f.server,"creative.project/practice/element/0"));CHECK(UmiCreativeWorkspaceReload(f.workspace)==UMI_STATUS_PARSE_ERROR);OK(UmiCreativeWorkspaceCopy(f.workspace,f.other));CHECK(f.other->elementCount==1U);
return 0;
}

/* workspace.corrupt_metadata: exercise the public behaviour named by this regression. */
static int Case30(void)
{
OK(Start(false));OK(umi_data_server_set(f.server,"creative.project/practice/meta/0","not-a-project\n"));CHECK(UmiCreativeWorkspaceReload(f.workspace)==UMI_STATUS_PARSE_ERROR);CHECK(UmiCreativeWorkspaceCreate(f.server,f.project,&f.second)==UMI_STATUS_PARSE_ERROR);
return 0;
}

/* workspace.delete_records_undo: exercise the public behaviour named by this regression. */
static int Case31(void)
{
OK(Start(false));OK(Add("a",0,0,20,20));OK(Add("b",30,0,20,20));OK(Save());CHECK(umi_data_server_count(f.server)==3U);OK(UmiCreativeProjectRemoveElement(f.project,"a"));OK(Save());CHECK(umi_data_server_count(f.server)==2U);OK(UmiCreativeWorkspaceUndo(f.workspace,f.project->revision));OK(UmiCreativeWorkspaceCopy(f.workspace,f.project));CHECK(f.project->elementCount==2U&&umi_data_server_count(f.server)==3U);
return 0;
}

/* workspace.maximum_revision: exercise the public behaviour named by this regression. */
static int Case32(void)
{
OK(Start(false));char wire[4096];f.project->revision=UINT64_MAX;OK(UmiCreativeWireProjectWrite(f.project,wire,sizeof(wire)));OK(umi_data_server_set(f.server,"creative.project/practice/meta/0",wire));OK(UmiCreativeWorkspaceReload(f.workspace));OK(UmiCreativeWorkspaceCopy(f.workspace,f.project));OK(Save());f.project->settings.tempoBpm=123U;CHECK(Save()==UMI_STATUS_CAPACITY_EXCEEDED);
return 0;
}

/* wire.round_trip: exercise the public behaviour named by this regression. */
static int Case33(void)
{
OK(UmiCreativeProjectStarter(f.project,"media"));f.project->revision=17U;char wire[4096];OK(UmiCreativeWireProjectWrite(f.project,wire,sizeof(wire)));OK(UmiCreativeWireProjectRead(wire,f.other));for(size_t i=0;i<f.project->elementCount;++i){OK(UmiCreativeWireElementWrite(&f.project->elements[i],wire,sizeof(wire)));OK(UmiCreativeWireElementRead(wire,&f.other->elements[i]));}CHECK(UmiCreativeProjectEqual(f.project,f.other));
return 0;
}

/* wire.strict_numbers: exercise the public behaviour named by this regression. */
static int Case34(void)
{
char wire[4096];f.project->revision=1U;OK(UmiCreativeWireProjectWrite(f.project,wire,sizeof(wire)));char *line=strchr(wire,'\n');CHECK(line!=NULL);memmove(line+2,line+1,strlen(line+1)+1U);line[1]='0';CHECK(UmiCreativeWireProjectRead(wire,f.other)==UMI_STATUS_PARSE_ERROR);
return 0;
}

/* wire.invalid_tail_and_short_buffer: exercise the public behaviour named by this regression. */
static int Case35(void)
{
char wire[4096];f.project->revision=1U;CHECK(UmiCreativeWireProjectWrite(f.project,wire,2U)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(wire[0]=='\0');OK(UmiCreativeWireProjectWrite(f.project,wire,sizeof(wire)));strcat(wire,"unexpected\n");CHECK(UmiCreativeWireProjectRead(wire,f.other)==UMI_STATUS_PARSE_ERROR);
return 0;
}

/* scene.linear_sample: exercise the public behaviour named by this regression. */
static int Case36(void)
{
OK(UmiCreativeProjectStarter(f.project,"media"));OK(UmiCreativeSceneSample(f.project,2000U,&f.frame));CHECK(fabs(f.frame.elements[1].x-290.0)<0.000001);CHECK(fabs(f.frame.elements[1].y-230.0)<0.000001);
return 0;
}

/* scene.step_exact_key: exercise the public behaviour named by this regression. */
static int Case37(void)
{
OK(Add("a",10,10,40,40));UmiCreativeKey keys[]={{0U,0,0,UMI_MEDIA_ANIMATION_STEP},{1000U,200,0,UMI_MEDIA_ANIMATION_STEP},{2000U,400,0,UMI_MEDIA_ANIMATION_LINEAR}};for(size_t i=0;i<3U;++i)OK(UmiCreativeProjectPutKey(f.project,"a",&keys[i]));OK(UmiCreativeSceneSample(f.project,999U,&f.frame));CHECK(f.frame.elements[0].x==10.0);OK(UmiCreativeSceneSample(f.project,1000U,&f.frame));CHECK(f.frame.elements[0].x==210.0);
return 0;
}

/* scene.hit_topmost: exercise the public behaviour named by this regression. */
static int Case38(void)
{
OK(Add("a",10,10,40,40));OK(Add("b",20,20,40,40));OK(UmiCreativeSceneSample(f.project,0U,&f.frame));size_t index;OK(UmiCreativeSceneHitTest(&f.frame,25.0,25.0,&index));CHECK(index==1U);f.frame.elements[1].visible=false;OK(UmiCreativeSceneHitTest(&f.frame,25.0,25.0,&index));CHECK(index==0U);CHECK(UmiCreativeSceneHitTest(&f.frame,50.0,50.0,&index)==UMI_STATUS_NOT_FOUND);
return 0;
}

/* scene.overlap_count: exercise the public behaviour named by this regression. */
static int Case39(void)
{
OK(Add("a",0,0,50,50));OK(Add("b",20,20,50,50));OK(Add("touch",70,20,20,20));OK(UmiCreativeSceneSample(f.project,0U,&f.frame));size_t count=0U;CHECK(UmiCreativeSceneOverlaps(&f.frame,NULL,0U,&count)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(count==1U);UmiCreativeOverlap pair;OK(UmiCreativeSceneOverlaps(&f.frame,&pair,1U,&count));CHECK(pair.first==0U&&pair.second==1U);f.frame.elements[1].kind=UMI_CREATIVE_TEXT;OK(UmiCreativeSceneOverlaps(&f.frame,&pair,1U,&count));CHECK(count==0U);
return 0;
}

/* scene.invalid_frame: exercise the public behaviour named by this regression. */
static int Case40(void)
{
OK(Add("a",0,0,50,50));OK(UmiCreativeSceneSample(f.project,0U,&f.frame));size_t index;f.frame.elements[0].x=NAN;CHECK(UmiCreativeSceneHitTest(&f.frame,1,1,&index)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiCreativeSceneSample(f.project,8001U,&f.frame)==UMI_STATUS_INVALID_ARGUMENT);
return 0;
}

/* animation.extreme_endpoints: exercise the public behaviour named by this regression. */
static int Case41(void)
{
OK(umi_media_animation_track_create(3U,&f.track));UmiMediaAnimationKeyframe key={0.0,-DBL_MAX,UMI_MEDIA_ANIMATION_LINEAR};OK(umi_media_animation_track_add(f.track,&key));key.time_seconds=2.0;key.value=DBL_MAX;OK(umi_media_animation_track_add(f.track,&key));double value;OK(umi_media_animation_track_sample(f.track,1.0,&value));CHECK(isfinite(value)&&value==0.0);OK(umi_media_animation_track_sample(f.track,0.5,&value));CHECK(isfinite(value)&&value<0.0);
return 0;
}

/* animation.clamp_and_step: exercise the public behaviour named by this regression. */
static int Case42(void)
{
OK(umi_media_animation_track_create(3U,&f.track));UmiMediaAnimationKeyframe key={0.0,10.0,UMI_MEDIA_ANIMATION_STEP};OK(umi_media_animation_track_add(f.track,&key));key.time_seconds=1.0;key.value=20.0;OK(umi_media_animation_track_add(f.track,&key));key.time_seconds=2.0;key.value=30.0;OK(umi_media_animation_track_add(f.track,&key));double value;OK(umi_media_animation_track_sample(f.track,-1.0,&value));CHECK(value==10.0);OK(umi_media_animation_track_sample(f.track,1.0,&value));CHECK(value==20.0);OK(umi_media_animation_track_sample(f.track,9.0,&value));CHECK(value==30.0);
return 0;
}

/* play.sweep_prevents_tunnelling: exercise the public behaviour named by this regression. */
static int Case43(void)
{
OK(UmiCreativeProjectStarter(f.project,"games"));OK(UmiCreativePlayBegin(f.project,"player",&f.play));UmiCreativeMotion motion;OK(UmiCreativePlayMove(f.project,&f.play,INT32_MAX,0,&motion));CHECK(f.play.x==220&&motion.blocked&&strcmp(motion.blockerId,"wall")==0);CHECK(f.project->elements[0].x==40);
return 0;
}

/* play.canvas_and_single_axis: exercise the public behaviour named by this regression. */
static int Case44(void)
{
OK(UmiCreativeProjectStarter(f.project,"games"));OK(UmiCreativePlayBegin(f.project,"player",&f.play));UmiCreativeMotion motion;CHECK(UmiCreativePlayMove(f.project,&f.play,1,1,&motion)==UMI_STATUS_INVALID_ARGUMENT);OK(UmiCreativePlayMove(f.project,&f.play,INT32_MIN,0,&motion));CHECK(f.play.x==0&&motion.blocked);
return 0;
}

/* play.touch_and_move_away: exercise the public behaviour named by this regression. */
static int Case45(void)
{
OK(UmiCreativeProjectStarter(f.project,"games"));OK(UmiCreativePlayBegin(f.project,"player",&f.play));UmiCreativeMotion motion;OK(UmiCreativePlayMove(f.project,&f.play,180,0,&motion));CHECK(!motion.blocked&&f.play.x==220);OK(UmiCreativePlayMove(f.project,&f.play,1,0,&motion));CHECK(motion.blocked&&motion.movedX==0);OK(UmiCreativePlayMove(f.project,&f.play,-10,0,&motion));CHECK(f.play.x==210&&!motion.blocked);
return 0;
}

/* play.stale_revision: exercise the public behaviour named by this regression. */
static int Case46(void)
{
OK(UmiCreativeProjectStarter(f.project,"games"));OK(UmiCreativePlayBegin(f.project,"player",&f.play));++f.project->revision;UmiCreativeMotion motion;CHECK(UmiCreativePlayMove(f.project,&f.play,1,0,&motion)==UMI_STATUS_BUSY);
return 0;
}

/* play.reject_animated_scene: exercise the public behaviour named by this regression. */
static int Case47(void)
{
OK(UmiCreativeProjectStarter(f.project,"games"));UmiCreativeKey key={0};OK(UmiCreativeProjectPutKey(f.project,"wall",&key));CHECK(UmiCreativePlayBegin(f.project,"player",&f.play)==UMI_STATUS_NOT_IMPLEMENTED);
return 0;
}

/* export.svg_escaping: exercise the public behaviour named by this regression. */
static int Case48(void)
{
OK(Add("a",0,0,100,50));(void)snprintf(f.project->elements[0].label,sizeof(f.element.label),"<script> & \"label\"");OK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_SVG,0U,&f.exported));const char *text=(const char *)f.exported.bytes;CHECK(strstr(text,"&lt;script&gt;")!=NULL&&strstr(text,"<script>")==NULL);CHECK(strstr(text,"&amp;")!=NULL);
return 0;
}

/* export.svg_physical_units: exercise the public behaviour named by this regression. */
static int Case49(void)
{
OK(UmiCreativeProjectStarter(f.project,"kitchen"));OK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_SVG,0U,&f.exported));CHECK(strstr((char *)f.exported.bytes,"3000mm")!=NULL);
return 0;
}

/* export.html_inert_and_branded: exercise the public behaviour named by this regression. */
static int Case50(void)
{
OK(UmiCreativeProjectStarter(f.project,"web-studio"));OK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_HTML,0U,&f.exported));const char *text=(const char *)f.exported.bytes;CHECK(strstr(text,"default-src 'none'")!=NULL);CHECK(strstr(text,"data:image/png;base64,")!=NULL);CHECK(strstr(text,"<script")==NULL&&strstr(text,"<form")==NULL);
return 0;
}

/* export.storyboard_three_frames: exercise the public behaviour named by this regression. */
static int Case51(void)
{
OK(UmiCreativeProjectStarter(f.project,"media"));OK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_STORYBOARD,0U,&f.exported));const char *at=(char *)f.exported.bytes;size_t count=0U;while((at=strstr(at,"<figure>"))!=NULL){++count;at+=8;}CHECK(count==3U);CHECK(strstr((char *)f.exported.bytes,"4000 ms")!=NULL);
return 0;
}

/* export.output_ownership: exercise the public behaviour named by this regression. */
static int Case52(void)
{
OK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_SVG,0U,&f.exported));unsigned char *owned=f.exported.bytes;CHECK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_SVG,0U,&f.exported)==UMI_STATUS_INVALID_STATE);CHECK(f.exported.bytes==owned);UmiCreativeExportFree(&f.exported);UmiCreativeExportFree(&f.exported);CHECK(f.exported.bytes==NULL&&f.exported.size==0U);
return 0;
}

/* export.invalid_input: exercise the public behaviour named by this regression. */
static int Case53(void)
{
CHECK(UmiCreativeExportBuild(f.project,(UmiCreativeExportKind)99,0U,&f.exported)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_WAVE,0U,&f.exported)==UMI_STATUS_INVALID_STATE);CHECK(f.exported.bytes==NULL);
return 0;
}

/* export.wave_header_and_silence: exercise the public behaviour named by this regression. */
static int Case54(void)
{
OK(UmiCreativeProjectStarter(f.project,"music"));OK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_WAVE,0U,&f.exported));CHECK(f.exported.size==44U+8U*48000U*2U);CHECK(memcmp(f.exported.bytes,"RIFF",4U)==0);CHECK(memcmp(f.exported.bytes+8U,"WAVEfmt ",8U)==0);CHECK(Read32(f.exported.bytes+24U)==48000U);CHECK(Read32(f.exported.bytes+40U)==f.exported.size-44U);bool nonzero=false;for(size_t i=44U;i<96000U;++i)if(f.exported.bytes[i]!=0U)nonzero=true;CHECK(nonzero);for(size_t i=44U+3U*48000U*2U;i<f.exported.size;++i)CHECK(f.exported.bytes[i]==0U);
return 0;
}

/* export.wave_polyphony_bound: exercise the public behaviour named by this regression. */
static int Case55(void)
{
UmiCreativeNote note={.durationTicks=480U,.pitch=69U,.velocity=127U};for(size_t i=0;i<64U;++i){(void)snprintf(note.id,sizeof(note.id),"voice-%zu",i);OK(UmiCreativeProjectPutNote(f.project,&note,false));}OK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_WAVE,0U,&f.exported));for(size_t i=44U;i<f.exported.size;i+=2U){uint16_t bits=(uint16_t)((unsigned)f.exported.bytes[i]|((unsigned)f.exported.bytes[i+1U]<<8U));int32_t value=bits<=32767U?(int32_t)bits:(int32_t)bits-65536;CHECK(value>=-26215&&value<=26215);}
return 0;
}

/* export.write_new_no_overwrite: exercise the public behaviour named by this regression. */
static int Case56(void)
{
OK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_SVG,0U,&f.exported));char path[4096];CHECK(AbsoluteOutput(path,sizeof(path),"export.svg"));(void)remove(path);OK(UmiCreativeExportWriteNew(&f.exported,path));CHECK(UmiCreativeExportWriteNew(&f.exported,path)==UMI_STATUS_ALREADY_EXISTS);CHECK(remove(path)==0);CHECK(UmiCreativeExportWriteNew(&f.exported,"relative.svg")==UMI_STATUS_INVALID_ARGUMENT);
return 0;
}

/* sqlite.restart: exercise the public behaviour named by this regression. */
static int Case57(void)
{
#ifndef UMICOM_HAS_SQLITE
return 77;
#else
OK(Start(true));OK(UmiCreativeProjectStarter(f.project,"media"));OK(Save());UmiCreativeWorkspaceDestroy(f.workspace);f.workspace=NULL;umi_data_server_destroy(f.server);f.server=NULL;OK(umi_data_server_create_sqlite("fixture.sqlite3",&f.server));OK(UmiCreativeWorkspaceOpen(f.server,"practice",&f.workspace));OK(UmiCreativeWorkspaceCopy(f.workspace,f.other));CHECK(UmiCreativeProjectEqual(f.project,f.other)&&f.other->revision==2U);
#endif
return 0;
}

/* sqlite.atomic_failure: exercise the public behaviour named by this regression. */
static int Case58(void)
{
#ifndef UMICOM_HAS_SQLITE
return 77;
#else
OK(Start(true));OK(Add("a",0,0,50,50));OK(Add("b",100,0,50,50));OK(Save());uint64_t revision=f.project->revision;OK(umi_data_server_execute(f.server,"CREATE TRIGGER stop_second BEFORE UPDATE ON umicom_kv WHEN NEW.key='creative.project/practice/element/1' BEGIN SELECT RAISE(ABORT,'injected'); END;"));f.project->elements[0].x=20;f.project->elements[1].x=140;CHECK(Save()!=UMI_STATUS_OK);OK(UmiCreativeWorkspaceCopy(f.workspace,f.other));CHECK(f.other->revision==revision&&f.other->elements[0].x==0);OK(UmiCreativeWorkspaceOpen(f.server,"practice",&f.second));OK(UmiCreativeWorkspaceCopy(f.second,f.other));CHECK(f.other->elements[0].x==0&&f.other->elements[1].x==100);CHECK(!umi_data_server_in_transaction(f.server));
#endif
return 0;
}

/* sqlite.rollback_failure_poison: exercise the public behaviour named by this regression. */
static int Case59(void)
{
#ifndef UMICOM_HAS_SQLITE
return 77;
#else
OK(Start(true));OK(Add("a",0,0,50,50));OK(Save());OK(umi_data_server_execute(f.server,"CREATE TRIGGER rollback_all BEFORE UPDATE ON umicom_kv BEGIN SELECT RAISE(ROLLBACK,'injected'); END;"));f.project->elements[0].x=20;CHECK(Save()==UMI_STATUS_INVALID_STATE);UmiCreativeHistory history;OK(UmiCreativeWorkspaceHistory(f.workspace,&history));CHECK(history.recoveryRequired);CHECK(UmiCreativeWorkspaceCopy(f.workspace,f.other)==UMI_STATUS_INVALID_STATE);CHECK(UmiCreativeWorkspaceReload(f.workspace)==UMI_STATUS_INVALID_STATE);
#endif
return 0;
}

/* sqlite.two_connections: exercise the public behaviour named by this regression. */
static int Case60(void)
{
#ifndef UMICOM_HAS_SQLITE
return 77;
#else
OK(Start(true));OK(umi_data_server_create_sqlite("fixture.sqlite3",&f.secondServer));OK(UmiCreativeWorkspaceOpen(f.secondServer,"practice",&f.second));OK(Add("a",0,0,50,50));OK(Save());OK(UmiCreativeWorkspaceCopy(f.second,f.other));f.other->settings.tempoBpm=150;CHECK(UmiCreativeWorkspaceReplace(f.second,f.other,1U)==UMI_STATUS_BUSY);OK(UmiCreativeWorkspaceReload(f.second));OK(UmiCreativeWorkspaceCopy(f.second,f.other));CHECK(f.other->revision==2U);
#endif
return 0;
}

/* sqlite.corrupt_row_rejected: exercise the public behaviour named by this regression. */
static int Case61(void)
{
#ifndef UMICOM_HAS_SQLITE
return 77;
#else
OK(Start(true));OK(Add("a",0,0,50,50));OK(Save());OK(umi_data_server_execute(f.server,"UPDATE umicom_kv SET value='invalid' WHERE key='creative.project/practice/element/0';"));CHECK(UmiCreativeWorkspaceReload(f.workspace)==UMI_STATUS_PARSE_ERROR);CHECK(UmiCreativeWorkspaceReplace(f.workspace,f.project,f.project->revision)==UMI_STATUS_PARSE_ERROR);
#endif
return 0;
}

/* workspace.content_conflict_same_revision: exercise the public behaviour named by this regression. */
static int Case62(void)
{
OK(Start(false));char wire[4096];*f.other=*f.project;(void)snprintf(f.other->settings.title,sizeof(f.other->settings.title),"Changed by another writer");OK(UmiCreativeWireProjectWrite(f.other,wire,sizeof(wire)));OK(umi_data_server_set(f.server,"creative.project/practice/meta/0",wire));CHECK(Save()==UMI_STATUS_BUSY);
return 0;
}

/* workspace.notes_and_keys_round_trip: exercise the public behaviour named by this regression. */
static int Case63(void)
{
OK(UmiCreativeProjectStarter(f.project,"music"));UmiCreativeKey key={0U,0,0,UMI_MEDIA_ANIMATION_STEP};OK(UmiCreativeProjectPutKey(f.project,"heading",&key));OK(Start(false));OK(UmiCreativeWorkspaceOpen(f.server,"practice",&f.second));OK(UmiCreativeWorkspaceCopy(f.second,f.other));CHECK(UmiCreativeProjectEqual(f.project,f.other));
return 0;
}

/* wire.adversarial_bytes: exercise the public behaviour named by this regression. */
static int Case64(void)
{
char valid[4096],wire[4096];f.project->revision=1U;OK(UmiCreativeWireProjectWrite(f.project,valid,sizeof(valid)));uint32_t state=0x31415926U;size_t length=strlen(valid);for(unsigned i=0U;i<5000U;++i){memcpy(wire,valid,length+1U);state=state*1664525U+1013904223U;size_t at=state%(length+1U);wire[at]=(char)(state>>24U);UmiStatus status=UmiCreativeWireProjectRead(wire,f.other);CHECK(status==UMI_STATUS_OK||status==UMI_STATUS_PARSE_ERROR);UmiCreativeElement element;UmiCreativeNote note;(void)UmiCreativeWireElementRead(wire,&element);(void)UmiCreativeWireNoteRead(wire,&note);}
return 0;
}

/* play.independent_integer_sweep: exercise the public behaviour named by this regression. */
static int Case65(void)
{
OK(UmiCreativeProjectStarter(f.project,"games"));uint32_t random=37U;for(unsigned trial=0U;trial<1000U;++trial){random=random*1664525U+1013904223U;int32_t distance=(int32_t)(random%2000U);OK(UmiCreativePlayBegin(f.project,"player",&f.play));int32_t expected=f.play.x;for(int32_t step=0;step<distance;++step){if(expected+40>=260)break;++expected;}UmiCreativeMotion motion;OK(UmiCreativePlayMove(f.project,&f.play,distance,0,&motion));CHECK(f.play.x==expected);}
return 0;
}

/* export.maximum_duration: exercise the public behaviour named by this regression. */
static int Case66(void)
{
f.project->settings.durationMs=30000U;UmiCreativeNote note={.id="brief",.durationTicks=480U,.pitch=69U,.velocity=80U};OK(UmiCreativeProjectPutNote(f.project,&note,false));OK(UmiCreativeExportBuild(f.project,UMI_CREATIVE_EXPORT_WAVE,0U,&f.exported));CHECK(f.exported.size==2880044U&&f.exported.size<=UMI_CREATIVE_EXPORT_LIMIT);
return 0;
}

static const struct {const char *name;int (*run)(void);} cases[]={
    {"project.initialise",Case0},
    {"project.invalid_identity",Case1},
    {"project.utf8_labels",Case2},
    {"project.malformed_utf8",Case3},
    {"project.settings_boundaries",Case4},
    {"project.add_replace_remove",Case5},
    {"project.failed_append_unchanged",Case6},
    {"project.failed_replace_unchanged",Case7},
    {"project.element_capacity",Case8},
    {"project.layer_order",Case9},
    {"project.unit_change_guard",Case10},
    {"project.keys_start_at_zero",Case11},
    {"project.key_sort_update",Case12},
    {"project.key_bounds",Case13},
    {"project.key_capacity",Case14},
    {"project.note_timing",Case15},
    {"project.note_invalid",Case16},
    {"project.note_capacity",Case17},
    {"project.failed_note_append_unchanged",Case18},
    {"project.starters",Case19},
    {"workspace.create_open",Case20},
    {"workspace.undo_redo",Case21},
    {"workspace.undo_branch",Case22},
    {"workspace.history_capacity",Case23},
    {"workspace.noop_padding",Case24},
    {"workspace.expected_revision",Case25},
    {"workspace.stale_handle",Case26},
    {"workspace.reload_clears_history",Case27},
    {"workspace.foreign_transaction",Case28},
    {"workspace.missing_child",Case29},
    {"workspace.corrupt_metadata",Case30},
    {"workspace.delete_records_undo",Case31},
    {"workspace.maximum_revision",Case32},
    {"wire.round_trip",Case33},
    {"wire.strict_numbers",Case34},
    {"wire.invalid_tail_and_short_buffer",Case35},
    {"scene.linear_sample",Case36},
    {"scene.step_exact_key",Case37},
    {"scene.hit_topmost",Case38},
    {"scene.overlap_count",Case39},
    {"scene.invalid_frame",Case40},
    {"animation.extreme_endpoints",Case41},
    {"animation.clamp_and_step",Case42},
    {"play.sweep_prevents_tunnelling",Case43},
    {"play.canvas_and_single_axis",Case44},
    {"play.touch_and_move_away",Case45},
    {"play.stale_revision",Case46},
    {"play.reject_animated_scene",Case47},
    {"export.svg_escaping",Case48},
    {"export.svg_physical_units",Case49},
    {"export.html_inert_and_branded",Case50},
    {"export.storyboard_three_frames",Case51},
    {"export.output_ownership",Case52},
    {"export.invalid_input",Case53},
    {"export.wave_header_and_silence",Case54},
    {"export.wave_polyphony_bound",Case55},
    {"export.write_new_no_overwrite",Case56},
    {"sqlite.restart",Case57},
    {"sqlite.atomic_failure",Case58},
    {"sqlite.rollback_failure_poison",Case59},
    {"sqlite.two_connections",Case60},
    {"sqlite.corrupt_row_rejected",Case61},
    {"workspace.content_conflict_same_revision",Case62},
    {"workspace.notes_and_keys_round_trip",Case63},
    {"wire.adversarial_bytes",Case64},
    {"play.independent_integer_sweep",Case65},
    {"export.maximum_duration",Case66},
};

int main(int argc,char **argv)
{
    if(argc!=2){fprintf(stderr,"Pass exactly one registered case name.\n");return 2;}
    size_t found=sizeof(cases)/sizeof(cases[0]);
    for(size_t i=0;i<sizeof(cases)/sizeof(cases[0]);++i)if(strcmp(argv[1],cases[i].name)==0){found=i;break;}
    if(found==sizeof(cases)/sizeof(cases[0]))return 2;
    f.project=calloc(1U,sizeof(*f.project));f.other=calloc(1U,sizeof(*f.other));
    if(f.project==NULL||f.other==NULL){free(f.project);free(f.other);return 2;}
    UmiCreativeSettings settings={.title="Practice design",.unit=UMI_CREATIVE_PIXELS,.width=960,.height=600,.durationMs=8000U,.tempoBpm=120U};
    int result=UmiCreativeProjectInit(f.project,"practice",&settings)==UMI_STATUS_OK?cases[found].run():2;
    UmiCreativeExportFree(&f.exported);umi_media_animation_track_destroy(f.track);
    UmiCreativeWorkspaceDestroy(f.third);UmiCreativeWorkspaceDestroy(f.second);UmiCreativeWorkspaceDestroy(f.workspace);
    umi_data_server_destroy(f.secondServer);umi_data_server_destroy(f.server);
    free(f.other);free(f.project);(void)remove("fixture.sqlite3");(void)remove("fixture.sqlite3-journal");
    if(result==0)printf("PASS %s\n",argv[1]);
    return result;
}
