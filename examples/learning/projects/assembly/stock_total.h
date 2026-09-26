/* Umicom Framework teaching project: assembly / stock_total.h
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#ifndef UMICOM_LESSON_STOCK_TOTAL_H
#define UMICOM_LESSON_STOCK_TOTAL_H
#include <stddef.h>
#include <stdint.h>
/* Sum modulo 2^32. count must describe readable uint32_t elements. A null
 * pointer is allowed only when count is zero. No ownership transfer. */
uint32_t StockTotalC(const uint32_t *values,size_t count);
uint32_t StockTotal(const uint32_t *values,size_t count);
#endif
