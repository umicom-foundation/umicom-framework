/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/game/test_grid_path.c
 *
 * PURPOSE:
 *   Verify shortest paths, blocked routes, search budgets and transactional output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/game/grid_path.h"
#include <stdio.h>
#include <stdint.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d\n", __LINE__); return 1; } } while (0)
int main(void)
{
    uint8_t grid[12] = {0,0,0,0, 1,1,1,0, 0,0,0,0};
    size_t queue[12], parents[12], path[13], count = 999;
    for (size_t i=0; i<13; ++i) path[i]=777;
    CHECK(UmiGameGridPath(grid,12,4,3,0,8,12,queue,parents,12,path,13,&count)==UMI_STATUS_OK);
    CHECK(count==9 && path[0]==0 && path[3]==3 && path[4]==7 && path[8]==8 && path[9]==777);
    for(size_t i=1;i<count;++i) {
        size_t a=path[i-1],b=path[i];
        CHECK(!grid[b] && ((a/4==b/4 && (a+1==b || b+1==a)) || a+4==b || b+4==a));
    }
    path[0]=777; count=999;
    CHECK(UmiGameGridPath(grid,12,4,3,0,8,1,queue,parents,12,path,13,&count)==UMI_STATUS_BUSY);
    CHECK(path[0]==777 && count==999);
    CHECK(UmiGameGridPath(grid,12,4,3,0,8,12,queue,parents,12,path,8,&count)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(path[0]==777 && count==999);
    grid[7]=1;
    CHECK(UmiGameGridPath(grid,12,4,3,0,8,12,queue,parents,12,path,13,&count)==UMI_STATUS_NOT_FOUND);
    CHECK(path[0]==777 && count==999);
    CHECK(UmiGameGridPath(grid,12,4,3,0,0,1,queue,parents,12,path,13,&count)==UMI_STATUS_OK);
    CHECK(count==1 && path[0]==0);
    CHECK(UmiGameGridPath(grid,12,SIZE_MAX,3,0,0,1,queue,parents,12,path,13,&count)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGameGridPath(grid,11,4,3,0,0,1,queue,parents,12,path,13,&count)==UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}
