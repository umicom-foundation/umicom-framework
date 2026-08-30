/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/hit_test.h
 *
 * PURPOSE:
 *   Map viewport pixel coordinates to logical row and column indexes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_HIT_TEST_H
#define UMICOM_UI_ENTERPRISE_HIT_TEST_H

#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntHit { size_t row,column; int valid; } UmiUiEntHit;
UmiUiEntHit umi_ui_ent_hit_test_uniform(int32_t x,int32_t y,int32_t row_height,int32_t column_width,size_t row_offset,size_t column_offset,size_t rows,size_t columns);

#endif
