#define LOG_LEV 0
#include "sys_inc.h"
#include "hal.h"
#include "user_config.h"
#include "cli.h"
#include "console.h"
#include "key.h"

#if DEBUG_LOG_EN
cli_comm_t comm;

AT_SRAM(.console)
void k0_handler(int argc, char **argv)
{
    logi("%s", __func__);
}

AT_SRAM(.console)
void k1_handler(int argc, char **argv)
{
    logi("%s", __func__);
}

AT_SRAM(.console)
void k2_handler(int argc, char **argv)
{
    logi("%s", __func__);
}

static struct cli_command console_cmds[] = {
	{"k0", NULL,        k0_handler},
	{"k1", NULL,        k1_handler},
	{"k2", "<on/off>",  k2_handler},
};

OAL_QUEUE uart_rx_queue;

AT_SRAM(.console)
uint32_t uart_rx_do()
{
    uint32_t ret = 0;
    OAL_queue_recv(&uart_rx_queue, &ret, OAL_WAIT_FOREVER);
    return ret;
}

AT(.startup_seg)
static char console_rx(char *ch)
{
    *ch = (char)uart_rx_do();
    return 1;
}

AT_SRAM(.console)
void uart_rx_cbk(uint32_t data)
{
    uart2_printf("%s\r\n",data);
    OAL_queue_send(&uart_rx_queue, &data, OAL_WAIT_FOREVER);
}

static void console_uart_init()
{
    HAL_uart_register_rx_callback(UART_ID_02, uart_rx_cbk);
    uart2_printf("console_uart_init\r\n");
    OAL_QUEUE_POOL pool;
    pool.size = 20 * sizeof(uint32_t);
    if(OAL_SUCCESS != OAL_queue_create(&uart_rx_queue, 0, sizeof(uint32_t), &pool))
        return;
}

void console_init()
{
    comm.rx = console_rx;
    comm.tx = xprintf +1; // rom function must +1 for run as function pointer.
    comm.init = console_uart_init;

     //初始化收发控制台
    cli_init(&comm);

    cli_register_commands(&console_cmds[0], sizeof(console_cmds) / sizeof(struct cli_command));
}
#endif
