![Umicom](../../resources/brand/umicom-logo.svg)

# Measure decoded RGB channel distributions

Use UmiAiTrainingImageHistogramRgb8 to obtain exact frequencies for a bounded
RGB8 view. The implementation is shared by Umicom::ai and Umicom::AiTraining.
Native Media or training clients can inspect decoded data without owning a
second counting implementation. This API does not decode an image or run a model.

## Represent the input

Include umicom/ai/training/image_histogram.h. A UmiAiTrainingRgb8View borrows
RGB bytes in top-to-bottom rows and red/green/blue order. width and height are
positive. row_stride_bytes describes the distance between row starts. The
complete source extent must cover every visible pixel; final-row padding is
not required. A borrowed crop can retain a larger original stride.

~~~c
const uint8_t pixels[] = {0, 10, 255, 255, 20, 0};
const UmiAiTrainingRgb8View view = {pixels, sizeof pixels, 2U, 1U, 6U};
UmiAiTrainingRgb8Histogram histogram;
UmiStatus status = UmiAiTrainingImageHistogramRgb8(&view, &histogram);
/* Only after OK: pixel_count is 2; red bins 0 and 255 each contain 1. */
~~~

## Read the report

bins[0] is red, bins[1] green and bins[2] blue. Each array contains 256 counters,
indexed by the unsigned byte value. Each channel's bin sum equals pixel_count,
not three times pixel_count. All unused bins are zero on success. The maximum
admitted width*height*3 is UMI_AI_TRAINING_IMAGE_MAX_VALUES, currently 16384;
therefore a single counter cannot exceed 5461. The implementation checks that
this bound continues to fit its uint32_t counters.

This is an exact code-value histogram. It does not linearize sRGB, account for
alpha, convert color spaces, infer labels or preserve spatial relationships.
A horizontal mirror has the same counts; a crop can have different counts.
Do not use histogram equality as an image identity or perceptual-quality test.

## Storage and failure behavior

The shared ImageDescribeRgb8 function owns dimension, stride and overflow
admission. An incomplete byte extent returns CAPACITY_EXCEEDED; null pointers,
zero dimensions and too-short row stride return INVALID_ARGUMENT. Other shared
size limits also return CAPACITY_EXCEEDED. All normal refusals leave the complete
output unchanged. Success replaces the result after all visible pixels have
been counted. The local candidate uses fixed automatic storage of roughly
three KiB plus metadata; no heap allocation occurs.

The caller owns valid, stable, disjoint view, source and result objects and
serializes their use. A declared length is not proof that an arbitrary pointer
is accessible. Nothing is retained after the call. File I/O, dataset aggregation,
random sampling, GUI rendering and model training are separate responsibilities.

## Learn and qualify

The Bits2Banking labs/framework/image-histogram laboratory is a narrow source
consumer with checked expected frequencies, crop composition and refusal
behavior. Framework tests/image_histogram covers literal distributions,
padding, mirrors, maximum sizes and independent count conservation. Keep exact
source and test evidence when qualifying a release. An installed SDK consumer
and application workflow require separate checks; source composition alone does
not prove their exports, DLL deployment or interface behavior.
