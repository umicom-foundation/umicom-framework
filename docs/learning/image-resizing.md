![Umicom](../../resources/brand/umicom-logo.svg)

# Resize prepared RGB pixels with explicit sampling

Resizing changes an image's pixel dimensions. Nearest-neighbor sampling copies
one source pixel for each output pixel. It does not invent recovered detail.
Framework provides UmiAiTrainingImageResizeNearestRgb8 for already decoded,
bounded RGB8 views used by native model-input preparation.

## Coordinate rule

For each destination coordinate d, select:

~~~text
floor((2*d + 1) * sourceSize / (2 * targetSize))
~~~

The rule maps pixel centers, with exact nearest-center ties going to the higher
source coordinate. Apply it separately to x and y. For four source pixels and
two output pixels, the selected source indices are 1 and 3. For two source
pixels and four output pixels, they are 0, 0, 1, 1.

This explicit rule matters: another resize library can use different alignment
or rounding. Matching output dimensions alone does not establish equal inputs.

## Native call

Include umicom/ai/training/image_resize.h and link Umicom::ai or the native
training target that owns the same implementation. Given a valid borrowed
source view and a separate writable output array:

~~~c
uint8_t reduced[6];
UmiStatus status = UmiAiTrainingImageResizeNearestRgb8(
    &source, 2, 1, reduced, sizeof reduced);
if (status != UMI_STATUS_OK) {
    /* The prior contents of reduced have not changed. */
}
~~~

Both complete source and target are validated by ImageDescribeRgb8. Positive
dimensions and the existing image-value bound apply to each independently.
Packed output has width * height * 3 bytes. Source row padding is skipped.
Successful output leaves any capacity tail untouched. Refused output is wholly
unchanged because no writes occur before complete admission.

Descriptors and pixel allocations must remain valid and stable throughout the
call. Input/output storage must not overlap. Metadata validation cannot prove
that a caller's pointer names a live allocation. The service retains no pointer,
allocates no image and performs no I/O.

## Compose explicit operations

1. Decode a supported file through a separately qualified decoder.
2. Retain ownership of the resulting RGB8 allocation.
3. Optionally borrow a checked crop through ImageCropRgb8.
4. Select dimensions and resampling policy appropriate to the model.
5. Resize into a separately owned packed array.
6. Pack that array through ImagePackRgb8 using the required HWC/CHW order and
   range. Resizing alone does not normalize bytes into floating-point values.
7. Keep labels, masks, captions and source-group membership consistent.

An arbitrary width/height pair can distort aspect ratio. This API does not
choose a crop, preserve aspect ratio, letterbox, adjust captions or update
bounding-box coordinates. Such policy belongs in an explicit higher-level plan.

## Information loss and model limits

The sequence black, white, black, white reduces to white, white under this
mapping. Enlarging it again produces four white pixels. A histogram makes the
lost dark values visible. Nearest sampling can alias fine textures; it is
unsuitable when the model contract requires an antialias or other interpolation
filter. C implementation and bounded storage do not change that mathematical
limitation.

The regression suite compares center selection against an independent distance
oracle for all source/target dimensions 1 through 7 on both axes. It also covers
padded rows, borrowed crops, tensor packing, complete-pixel copies, exact ties,
maximal admitted rows/columns, metadata overflow and transactional refusal.
These focused checks do not establish installed SDK behavior, Media UI behavior,
decoder compatibility or model quality.
