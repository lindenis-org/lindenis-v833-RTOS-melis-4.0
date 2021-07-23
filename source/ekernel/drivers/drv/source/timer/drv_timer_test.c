#include <interrupt.h>
#include <sunxi_drv_timer.h>

static irqreturn_t hal_timer_bad_irq_handle(int dummy, void *data)
{
    printf("irq %d interrupt!!\n", dummy);
    return 0;
}

static int timer_test(int argc, const char **argv)
{
    u32 irq_num, value = 0;

    drv_timer_init();
    /*
    typedef enum {
        TMR_CONTINUE_MODE,
        TMR_SINGLE_MODE,
    } drv_timer_mode;
    */
    drv_timer_set_pres(TIMER0, TMR_2_PRES);
    /*
    typedef enum {
        TMR_1_PRES,
        TMR_2_PRES,
        TMR_4_PRES,
        TMR_8_PRES,
        TMR_16_PRES,
        TMR_32_PRES,
        TMR_64_PRES,
        TMR_128_PRES,
    } hal_timer_pres;
    */
    drv_timer_set_mode(TIMER0, TMR_CONTINUE_MODE);
    drv_timer_set_interval(TIMER0, 0x0EE0);

    irq_num = drv_timer_id_to_irq(TIMER0);
    printf("irq_num = %ld\n", irq_num);
    drv_timer_irq_request(irq_num, hal_timer_bad_irq_handle, 0, NULL);
    drv_timer_irq_enable(irq_num);

    if (drv_timer_get_counter(TIMER0, &value) == DRV_TMR_STATUS_OK)
    {
        printf("value =0x%lx\n", value);
    }

    drv_timer_set_pres(TIMER1, TMR_2_PRES);
    drv_timer_set_mode(TIMER1, TMR_CONTINUE_MODE);
    drv_timer_set_interval(TIMER1, 0x0EE0);

    irq_num = drv_timer_id_to_irq(TIMER1);
    printf("irq_num = %ld\n", irq_num);
    drv_timer_irq_request(irq_num, hal_timer_bad_irq_handle, 0, NULL);
    drv_timer_irq_enable(irq_num);

    if (drv_timer_get_counter(TIMER1, &value) == DRV_TMR_STATUS_OK)
    {
        printf("value =0x%lx\n", value);
    }

    drv_timer_set_mode(TIMER0, TMR_SINGLE_MODE);
    return 0;
}
