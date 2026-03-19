#include "tile.h"
#include "eventunit.h"

#include "compare_utils.h"
#include "test_mem_layout.h"
#include "test_params.h"
#include "test_task_bin.h"

#define LEN 16

static int init_data(void *params)
{
    uint32_t offset;
    volatile test_params_t *test_params;

    test_params = (volatile test_params_t *) params;
    for(int i = 0; i < LEN; i++) {
        offset = i * sizeof(float16);

        mmio_fp16(SRC_BASE + offset) = (float16) ((float)(i+1)/4.0f);
    }

    test_params->addr_src = SRC_BASE;
    test_params->len = LEN;

    return 0;
}

static int run_spatz_task()
{
    int ret;
    eu_config_t eu_cfg;
    eu_controller_t eu_ctrl;

    eu_cfg.hartid = get_hartid();
    eu_ctrl.base = NULL,
    eu_ctrl.cfg = &eu_cfg,
    eu_ctrl.api = &eu_api,

    eu_init(&eu_ctrl);
    eu_spatz_init(&eu_ctrl, 0);

    spatz_init(SPATZ_BINARY_START);
    spatz_run_task_with_params(TEST_TASK, TEST_PARAMS_BASE);

    eu_spatz_wait(&eu_ctrl, WFE);

    ret = spatz_get_exit_code();

    spatz_clk_dis();

    return ret;
}

static bool run_test()
{
    int ret;
    bool check;
    volatile test_params_t *params;

    params = (volatile test_params_t *) TEST_PARAMS_BASE;

    ret = init_data(params);
    if (ret != 0) {
        printf("[CV32] Params initialization failed with error: %d\n", ret);
        return ret;
    }

    ret = run_spatz_task();
    if (ret != 0) {
        printf("[CV32] Spatz task FAILED with error: %d", ret);
        return ret;
    }

    return ret;
}

int main(void)
{
    int ret;

    printf("\n##################################### TEST TEST #####################################\n\n");

    ret = run_test();

    printf("\n##########################################################################################\n\n");

    return ret;
}
