/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/price_increment/test_bands.c
 * PURPOSE: Review exact boundaries and reject ambiguous or invalid price ladders atomically.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiTradingPriceIncrement bands[] = {
        {Amount(0, 0), Amount(1, 2)}, {Amount(10, 0), Amount(5, 2)}, {Amount(100, 0), Amount(1, 1)}};
    UmiTradingPriceIncrementReview review = {0};
    review.bandIndex = 99U;
    const char *mode = argv[1];
    if (!strcmp(mode, "edges"))
    {
        CHECK(UmiTradingPriceIncrementReviewAt(bands, 3U, Amount(999, 2), &review) == UMI_STATUS_OK);
        CHECK(review.bandIndex == 0U && review.aligned);
        CHECK(UmiTradingPriceIncrementReviewAt(bands, 3U, Amount(10, 0), &review) == UMI_STATUS_OK);
        CHECK(review.bandIndex == 1U && review.aligned);
        CHECK(UmiTradingPriceIncrementReviewAt(bands, 3U, Amount(1001, 2), &review) == UMI_STATUS_OK);
        CHECK(review.bandIndex == 1U && !review.aligned);
        CHECK(UmiTradingPriceIncrementReviewAt(bands, 3U, Amount(100, 0), &review) == UMI_STATUS_OK);
        CHECK(review.bandIndex == 2U && review.aligned);
    }
    else if (!strcmp(mode, "mixed-scale"))
    {
        bands[1].lowerBound = Amount(10000, 3);
        CHECK(UmiTradingPriceIncrementReviewAt(bands, 3U, Amount(1005, 2), &review) == UMI_STATUS_OK);
        CHECK(review.bandIndex == 1U && review.aligned);
    }
    else if (!strcmp(mode, "below"))
    {
        bands[0].lowerBound = Amount(1, 0);
        CHECK(UmiTradingPriceIncrementReviewAt(bands, 3U, Amount(99, 2), &review) == UMI_STATUS_UNAVAILABLE);
        CHECK(review.bandIndex == 99U);
    }
    else if (!strcmp(mode, "bad-order"))
    {
        bands[1].lowerBound = Amount(0, 0);
        CHECK(UmiTradingPriceIncrementReviewAt(bands, 3U, Amount(1, 0), &review) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(review.bandIndex == 99U);
    }
    else if (!strcmp(mode, "bad-increment"))
    {
        bands[1].increment = Amount(0, 0);
        CHECK(UmiTradingPriceIncrementsValidate(bands, 3U) == UMI_STATUS_INVALID_ARGUMENT);
        bands[1].increment = Amount(3, 0);
        CHECK(UmiTradingPriceIncrementsValidate(bands, 3U) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (!strcmp(mode, "bounds"))
    {
        CHECK(UmiTradingPriceIncrementsValidate(bands, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTradingPriceIncrementsValidate(bands, 65U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTradingPriceIncrementsValidate(NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTradingPriceIncrementReviewAt(bands, 3U, Amount(-1, 0), &review) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(review.bandIndex == 99U);
    }
    else
        return 2;
    return 0;
}
