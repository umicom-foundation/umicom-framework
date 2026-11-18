![Umicom](../../resources/brand/umicom-logo.svg)

# Measure RGB channel distributions

An RGB8 image stores three unsigned code values per pixel: red, green and blue,
usually from zero to 255. A histogram counts how often each code value occurs.
The moment service derives a minimum, maximum, mean and population variance
for each channel from those exact frequencies.

The mean is the sum of values divided by their count. Population variance is
the average squared distance from that mean; its denominator is N. It is not
the sample estimator that divides by N-1. A single pixel and a constant channel
have zero population variance. Variance uses squared code units, while the
mean and extrema use ordinary code units.

## Use the shared reader

1. Start with an already decoded, valid RGB8 view. Check dimensions, byte extent
   and row stride using the image-view contract.
2. Include umicom/ai/training/image_moments.h and link the AI component.
3. Call UmiAiTrainingImageMeasureRgb8 for an admitted view. It reuses the existing
   histogram reader; it does not add another pixel-reading implementation.
4. Check UMI_STATUS_OK before reading the output. A refusal leaves the previous
   result unchanged.
5. If you already have a histogram, call UmiAiTrainingImageHistogramMoments to
   avoid reading pixels again. All three channel sums must match pixel_count.

~~~c
const uint8_t pixels[] = {0,10,255, 255,30,255};
const UmiAiTrainingRgb8View view = {pixels,sizeof pixels,2U,1U,6U};
UmiAiTrainingRgb8Moments result;
UmiStatus status = UmiAiTrainingImageMeasureRgb8(&view,&result);
/* Inspect result only when status is UMI_STATUS_OK. */
~~~

For these two pixels, means are 127.5, 20 and 255. Population variances are
16256.25, 100 and 0. Blue is constant even though its numeric mean is high.
That statement is about channel code values, not perceived brightness.

## Understand the memory and arithmetic

The existing profile permits at most 16,384 RGB values, hence 5461 complete
pixels. The histogram adapter uses fixed automatic storage. The moment
calculation allocates nothing and retains no pointer. Row padding and pixels
outside an admitted crop do not contribute.

Weighted sums and squared sums use bounded integers. The variance numerator
N*sum(x*x)-sum(x)*sum(x) is formed exactly before conversion to double. This
avoids subtracting two already rounded floating-point moments for a nearly
constant channel. The source asserts the reviewed integer bound so enlarging
the image profile requires revisiting that proof. Final division can still
round its mathematical result.

Caller-created histograms are checked too. A zero count, a count beyond the
profile or inconsistent bin totals is refused. The result is not silently
repaired, and a failure in the last channel does not publish the first two.
Input and output must be valid, disjoint, stable objects for the whole call.

## Keep normalization a separate decision

These functions do not rescale pixels, linearize sRGB, compute a standard
deviation, estimate dataset statistics or train a model. Standard deviation
would be the square root of variance. Dividing a constant channel by its zero
standard deviation needs an explicit model-specific policy; do not hide that
choice in an inspection function.

A model might require fixed scaling to zero-to-one, signed scaling, or
standardization with statistics estimated from the training split. Those are
different contracts. Fitting statistics using evaluation images can leak
information across the intended split. Computing per-image statistics can
remove useful color or intensity information. Follow the actual model's
preprocessing specification rather than inferring it from this report.

Histograms discard spatial position. Images with very different arrangements
can share all these moments. The results do not identify objects, measure
semantic quality or prove that an image dataset is independent or licensed.
The Bits2Banking image-histogram lab includes a complete C example and an
illustrated beginner lesson about these distinctions.
