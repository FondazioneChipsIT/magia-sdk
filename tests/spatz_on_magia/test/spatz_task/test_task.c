#include "tile.h"
#include "test_params.h"

// #define LOGGING

static inline uint16_t get_raw(const _Float16 val)
{
    uint16_t raw;
    memcpy(&raw, &val, sizeof(raw));
    return raw;
}

static inline void print_vector_raw(const _Float16 *vec, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        printf("[CC]\t%d)\t%x\t(addr: %p)\n", i, get_raw(vec[i]), (void *)(vec + i));
    }
}

/**********************************************************************************************************************/

__attribute__((__noinline__)) static void test_vfmv_f_s(const _Float16 *src, const size_t len)
{
    volatile _Float16 first;
    const _Float16 *p_src;
    size_t avl;
    size_t vl;

    p_src = src;
    avl = len;

    for (; avl > 0; avl -= vl) {
        asm volatile ("vsetvli %0, %1, e16, m8, ta, ma" : "=r"(vl) : "r"(avl));
        asm volatile ("vle16.v v0, (%0)" :: "r"(p_src));
        asm volatile ("vfmv.f.s %0, v0" : "=f"(first));

        // asm volatile ("vmv.v.v v0, v0");                    /* dummy */
        // asm volatile ("fmv.s f1, %0" :: "f"(first) : "f1"); /* dummy */

#ifdef  LOGGING
        printf("fist: %x (expected: %x)\n", get_raw(first), get_raw(p_src[0]));
#endif
        p_src += vl;
    }

}

/**********************************************************************************************************************/

__attribute__((__noinline__)) static void test_fsgnj_h()
{
    _Float16 a;
    _Float16 b;
    _Float16 r1;
    _Float16 r2;

    a = 5.0f;
    b = -1.0f;

    asm volatile ("fsgnj.h %0, %1, %2" : "=f"(r1) : "f"(a), "f"(b));
    asm volatile ("fsgnj.h %0, %1, %2" : "=f"(r2) : "f"(b), "f"(a));

#ifdef LOGGING
    printf("a=%x    (0x4500 -> +5.0)\n", get_raw(a));
    printf("b=%x    (0xbc00 -> -1.0)\n", get_raw(b));
    printf("r1=%x   (0xc500 -> -5.0)\n", get_raw(r1));
    printf("r2=%x   (0x3c00 -> +1.0)\n", get_raw(r2));
#endif
}

/**********************************************************************************************************************/

__attribute__((__noinline__)) static _Float16 scalar_vfredsum(const _Float16 *src, const size_t len)
{
    _Float16 sum;

    sum = 0;
    for (int i = 0; i < len; i++)
        sum += src[i];

    return sum;
}

__attribute__((__noinline__)) static _Float16 rvv_vfredsum(const _Float16 *src, const size_t len)
{
    _Float16 ZERO_f = 0.0f;
    const _Float16 *p_src;
    volatile _Float16 sum;

    size_t original_avl;
    size_t avl;
    size_t vl;

    original_avl = len;
    p_src = src;
    avl = len;

    asm volatile ("vsetvli %0, %1, e16, m8, ta, ma" : "=r"(vl) : "r"(avl));
    asm volatile ("vfmv.v.f v0, %0" :: "f"(ZERO_f));
    asm volatile ("vfmv.v.f v8, %0" :: "f"(ZERO_f));

    for (; avl > 0; avl -= vl) {
        asm volatile ("vsetvli %0, %1, e16, m8, ta, ma" : "=r"(vl) : "r"(avl));
        asm volatile ("vle16.v v16, (%0)" :: "r"(p_src));
        asm volatile ("vfadd.vv v0, v0, v16");

        p_src += vl;
    }

    asm volatile ("vsetvli %0, %1, e16, m8, ta, ma" : "=r"(vl) : "r"(original_avl));
    asm volatile ("vfredosum.vs v8, v0, v8");
    asm volatile ("vfmv.f.s %0, v8" : "=f"(sum));

    return sum;
}

__attribute__((__noinline__)) static void test_vfredsum(const _Float16 *src, const size_t len)
{
    _Float16 scalar_sum;
    _Float16 rvv_sum;

    scalar_sum = scalar_vfredsum(src, len);
    rvv_sum = rvv_vfredsum(src, len);

#ifdef LOGGING
    if (scalar_sum != rvv_sum)
        printf("vfredsum test Failed! (computed: %x - expected: %x)\n", get_raw(rvv_sum), get_raw(scalar_sum));
    else
        printf("vfredsum test Success!\n");
#endif

}

/*********************************************************************************************/

int test_task(void)
{
    volatile test_params_t *params;
    uintptr_t params_addr;
    _Float16 *src;
    size_t len;

    params_addr = mmio32(SPATZ_DATA);
    params = (volatile test_params_t *) params_addr;

    src = (_Float16 *)params->addr_src;
    len = params->len;

    printf("===========================================\n");
    printf("[CC]\tSPATZ_TASKBIN:\t%p\n", (void *) mmio32(SPATZ_TASKBIN));
    printf("[CC]\tSPATZ_DATA:\t%p\n", (void *) mmio32(SPATZ_DATA));
    printf("===========================================\n");

    // print_vector_raw(src, len);

    test_vfmv_f_s(src, len);
    test_fsgnj_h();
    test_vfredsum(src, len);

    return 0;
}
