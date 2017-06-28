#include "src/encode1f.c"

#include "constants/1dFloat.h"
#include "utils/testMacros.h"
#include "utils/genSmoothRandNums.h"
#include "utils/hash32.c"
#include "zfpEndtoendBase.c"

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(when_seededRandomSmoothDataGenerated_expect_ChecksumMatches, setupFixedPrec, teardown),
    cmocka_unit_test_setup_teardown(given_1dFloatArray_when_ZfpCompressFixedPrecision_expect_BitstreamChecksumMatches, setupFixedPrec, teardown),
    cmocka_unit_test_setup_teardown(given_1dFloatArray_when_ZfpDecompressFixedPrecision_expect_ArrayChecksumMatches, setupFixedPrec, teardown),
    cmocka_unit_test_setup_teardown(given_1dFloatArray_when_ZfpCompressFixedRate_expect_BitstreamChecksumMatches, setupFixedRate, teardown),
    cmocka_unit_test_setup_teardown(given_1dFloatArray_when_ZfpDecompressFixedRate_expect_ArrayChecksumMatches, setupFixedRate, teardown),
    cmocka_unit_test_setup_teardown(given_1dFloatArray_when_ZfpCompressFixedRate_expect_CompressedBitrateComparableToChosenRate, setupFixedRate, teardown),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
