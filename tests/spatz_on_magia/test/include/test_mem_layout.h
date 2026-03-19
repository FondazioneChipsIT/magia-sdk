#ifndef TEST_H_
#define TEST_H_

#include "magia_tile_utils.h"
#include "magia_utils.h"
#include "test_params.h"

#define ALIGNMENT   4
/* Aligns the given address to 4-byte  */
#define ALIGN_4B(addr)  (((addr) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

#define L1_BASE_TILE    (L1_BASE + (get_hartid() * L1_TILE_OFFSET))
#define VEC_SIZE        (LEN * sizeof(float16))

#define TEST_PARAMS_BASE    L1_BASE_TILE
#define TEST_PARAMS_SIZE    ALIGN_4B(sizeof(test_params_t))

#define SRC_BASE  ALIGN_4B(TEST_PARAMS_BASE + TEST_PARAMS_SIZE)
#define SRC_SIZE  ALIGN_4B(VEC_SIZE)

#endif  /* TEST_H_ */
