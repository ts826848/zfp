#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdlib.h>

struct setupVars {
  Int* dataArr;
  void* buffer;
  zfp_stream* stream;
};

static int
setup(void **state)
{
  struct setupVars *bundle = malloc(sizeof(struct setupVars));
  assert_non_null(bundle);

  resetRandGen();

  bundle->dataArr = malloc(sizeof(Int) * BLOCK_SIZE);
  assert_non_null(bundle->dataArr);

  int i;
  for (i = 0; i < BLOCK_SIZE; i++) {
    bundle->dataArr[i] = nextSignedRand();
  }

  zfp_type type = ZFP_TYPE_INT;
  zfp_field* field;
  switch(DIMS) {
    case 1:
      field = zfp_field_1d(bundle->dataArr, type, 4);
      break;
    case 2:
      field = zfp_field_2d(bundle->dataArr, type, 4, 4);
      break;
    case 3:
      field = zfp_field_3d(bundle->dataArr, type, 4, 4, 4);
      break;
  }

  zfp_stream* stream = zfp_stream_open(NULL);
  zfp_stream_set_rate(stream, ZFP_RATE_PARAM_BITS, type, DIMS, 0);

  size_t bufsizeBytes = zfp_stream_maximum_size(stream, field);
  char* buffer = calloc(bufsizeBytes, sizeof(char));
  assert_non_null(buffer);

  bitstream* s = stream_open(buffer, bufsizeBytes);
  assert_non_null(s);

  zfp_stream_set_bit_stream(stream, s);
  zfp_stream_rewind(stream);
  zfp_field_free(field);

  bundle->buffer = buffer;
  bundle->stream = stream;

  *state = bundle;

  return 0;
}

static int
teardown(void **state)
{
  struct setupVars *bundle = *state;

  stream_close(bundle->stream->stream);
  zfp_stream_close(bundle->stream);
  free(bundle->buffer);
  free(bundle->dataArr);
  free(bundle);

  return 0;
}

static void
when_seededRandomDataGenerated_expect_ChecksumMatches(void **state)
{
  struct setupVars *bundle = *state;
  assert_int_equal(hashSignedArray(bundle->dataArr, BLOCK_SIZE, 1), CHECKSUM_ORIGINAL_DATA_BLOCK);
}

static void
_catFunc3(given_, DIM_INT_STR, Block_when_DecodeBlock_expect_ReturnValReflectsNumBitsReadFromBitstream)(void **state)
{
  struct setupVars *bundle = *state;
  zfp_stream* stream = bundle->stream;
  bitstream* s = zfp_stream_bit_stream(stream);

  _t2(zfp_encode_block, Int, DIMS)(stream, bundle->dataArr);
  zfp_stream_flush(stream);
  zfp_stream_rewind(stream);

  uint returnValBits = _t2(zfp_decode_block, Int, DIMS)(stream, bundle->dataArr);

  assert_int_equal(returnValBits, stream_rtell(s));
}

static void
_catFunc3(given_, DIM_INT_STR, Block_when_DecodeBlock_expect_ArrayChecksumMatches)(void **state)
{
  struct setupVars *bundle = *state;
  zfp_stream* stream = bundle->stream;
  bitstream* s = zfp_stream_bit_stream(stream);

  _t2(zfp_encode_block, Int, DIMS)(stream, bundle->dataArr);
  zfp_stream_flush(stream);
  zfp_stream_rewind(stream);

  Int* decodedDataArr = calloc(BLOCK_SIZE, sizeof(Int));
  assert_non_null(decodedDataArr);
  _t2(zfp_decode_block, Int, DIMS)(stream, decodedDataArr);

  UInt checksum = hashSignedArray(decodedDataArr, BLOCK_SIZE, 1);
  free(decodedDataArr);

  assert_int_equal(checksum, CHECKSUM_DECODED_BLOCK);
}