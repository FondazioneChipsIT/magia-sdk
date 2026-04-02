#ifndef ONNX_ELU_PARAMS_H_
#define ONNX_ELU_PARAMS_H_

#include <stdint.h>

typedef struct {
    uintptr_t add_alpha;
    uintptr_t addr_res;
    uintptr_t addr_exp;
    uintptr_t addr_src;
    uint32_t len;
} onnx_elu_params_t;

#endif  /* ONNX_ELU_PARAMS_H */
