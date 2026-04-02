#include "tile.h"
#include "onnx_elu_params.h"

static inline uint16_t get_raw(const _Float16 val)
{
    uint16_t raw;
    memcpy(&raw, &val, sizeof(raw));
    return raw;
}

static inline void print_vector_raw(const _Float16 *vec, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        printf("%d) %x\n", i, get_raw(vec[i]));
    }
}

static __attribute__((noinline)) void elu_forward(const _Float16 *src, _Float16 *dst, const _Float16 alpha, const size_t len)
{
    register _Float16 BIAS asm ("fs1") = 15360.0f;
    register _Float16 COEF asm ("fs2") = 1477.0f;
    register _Float16 ZERO asm ("fs3") = 0.0f;
    register _Float16 ONE  asm ("fs4") = 1.0f;
    register _Float16 MIN  asm ("fs5") = -5.0f;
    const _Float16 *p_src;
    _Float16 *p_dst;
    size_t avl;
    size_t vl;

    p_src = src;
    p_dst = dst;
    avl = len;

    /* TODO: remove me :) */
    uint32_t mask = 0;
    /* ----------------- */

    for (; avl > 0; avl -= vl) {
        asm volatile ("vsetvli %0, %1, e16, m8, ta, mu" : "=r"(vl) : "r"(avl));

        asm volatile ("vle16.v v8, (%0)" :: "r"(p_src));

        asm volatile ("vfmv.v.f v0, %0" :: "f"(ZERO));      /* TODO: is this needed? */
        asm volatile ("vmfge.vf v0, v8, %0" :: "f"(ZERO));

        /* TODO: remove me :) */
        printf("untouched:\n");
        print_vector_raw(p_dst, vl);
        /* ----------------- */

        asm volatile ("vse16.v v8, (%0), v0.t" :: "r"(p_dst));

        /* TODO: remove me :) */
        asm volatile ("vmv.x.s %0, v0" : "=r"(mask));
        printf("after store of non-negatives (vmfge: %x)\n", mask);
        print_vector_raw(p_dst, vl);
        /* ----------------- */

        /* ---------- fast exp approximation ---------- */
        asm volatile ("vfmv.v.f v0, %0" :: "f"(ZERO));      /* TODO: is this needed? */
        asm volatile ("vmflt.vf v0, v8, %0" :: "f"(ZERO));

        /* clamp for stability */
        asm volatile ("vfmax.vf v8, v8, %0, v0.t" :: "f"(MIN));

        asm volatile ("vfmul.vf v8, v8, %0, v0.t" :: "f"(COEF));
        asm volatile ("vfadd.vf v8, v8, %0, v0.t" :: "f"(BIAS));
        asm volatile ("vfcvt.rtz.xu.f.v v8, v8, v0.t");
        /* -------------------------------------------- */

        asm volatile ("vfsub.vf v8, v8, %0, v0.t" :: "f"(ONE));
        asm volatile ("vfmul.vf v8, v8, %0, v0.t" :: "f"(alpha));

        asm volatile ("vse16.v v8, (%0), v0.t" :: "r"(p_dst));

        /* TODO: remove me :) */
        mask = 0;
        asm volatile ("vmv.x.s %0, v0" : "=r"(mask));
        printf("after store of negatives (vmflt: %x)\n", mask);
        print_vector_raw(p_dst, vl);
        /* ----------------- */

        p_src += vl;
        p_dst += vl;
    }
}

int onnx_elu_task(void)
{
    volatile onnx_elu_params_t *params;
    uintptr_t params_addr;

    _Float16 alpha;
    _Float16 *src;
    _Float16 *dst;
    size_t len;

    params_addr = mmio32(SPATZ_DATA);
    params = (volatile onnx_elu_params_t *) params_addr;

    alpha = *(_Float16 *) params->add_alpha;
    src = (_Float16 *)params->addr_src;
    dst = (_Float16 *)params->addr_res;
    len = params->len;

    elu_forward(src, dst, alpha, len);

    return 0;
}
