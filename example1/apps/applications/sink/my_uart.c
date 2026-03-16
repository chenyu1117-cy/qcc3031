#include <stream.h>
#include <sink.h>
#include <source.h>
#include <string.h>
#include <panic.h>
#include <message.h>
#include <pio.h>
#include <vmal.h>
#include "sink_debug.h"
#include "my_uart.h"
#include "cq_cmd.h"  // 包含AT命令头文件

#ifdef DEBUG_UART
#define UART_DEBUG(x)               DEBUG(x)
#define UART_DEBUG_DUMP(buf,cnt)    do{uint32 idx=0;for(idx=0;idx<cnt;idx++){DEBUG(("%02X",buf[idx]));}DEBUG(("\n"));}while(0)
#else
#define UART_DEBUG(x)
#define UART_DEBUG_DUMP(buf,cnt)
#endif

#define PIO2BANK(pio) ((uint16)((pio) / 32))
#define PIO2MASK(pio) (1UL << ((pio) % 32))
UARTStreamTaskData theUARTStreamTask;

// 声明静态函数
static void parse_and_handle_line(const char *line, uint16 len);

// 静态函数定义
static void parse_and_handle_line(const char *line, uint16 len)
{
    recv_t recv;
    memset(&recv, 0, sizeof(recv));

    // 跳过可能的空白字符
    const char *p = line;
    while (len > 0 && (*p == ' ' || *p == '\t')) {
        p++;
        len--;
    }
    if (len == 0) return;

    // 检查是否以 "AT+" 开头（可选）
    if (len >= 3 && strncmp(p, "AT+", 3) == 0) {
        p += 3;
        len -= 3;
    }

    // 提取两位命令字母
    if (len >= 2 && ((p[0] >= 'A' && p[0] <= 'Z') || (p[0] >= '0' && p[0] <= '9')) &&
                    ((p[1] >= 'A' && p[1] <= 'Z') || (p[1] >= '0' && p[1] <= '9'))) {
        recv.cmd[0] = p[0];
        recv.cmd[1] = p[1];
        recv.cmd[2] = '\0';
        p += 2;
        len -= 2;
    } else {
        // 格式错误，忽略
        return;
    }

    // 跳过可能的 '=' 或空格，剩余部分作为参数
    while (len > 0 && (*p == '=' || *p == ' ' || *p == '\t')) {
        p++;
        len--;
    }
    if (len > 0) {
        // 复制参数（最大不超过 recv.param 大小）
        uint16 param_len = (len < sizeof(recv.param) - 1) ? len : sizeof(recv.param) - 1;
        memcpy(recv.param, p, param_len);
        recv.param[param_len] = '\0';
    }

    // 调用 cq_cmd 中的处理函数
    handle_at_command(&recv);
}

void uart_device_init(void)
{
    uart_pios_init();
    uart_data_stream_init();
    UART_DEBUG(("uart_device_init\n"));
}

//pio 1~8 15~23
#define UartTxPio 5
#define UartRxPio 4

void uart_pios_init(void)
{
    uint32 i;
    uint16 bank;
    uint32 mask;
    uint16 pio;
    struct
    {
            uint16 pio;
            pin_function_id func;
    } uart_pios[] = {{UartTxPio, UART_TX},
                    {UartRxPio, UART_RX}};

    for (i = 0; i < ARRAY_DIM(uart_pios); i++)
    {
            pio = uart_pios[i].pio;
            bank = PIO2BANK(pio);
            mask = PIO2MASK(pio);

            /* Setup Uart PIOs with strong pull-up */
            PanicNotZero(PioSetMapPins32Bank(bank, mask, 0));
            PanicFalse(PioSetFunction(pio, uart_pios[i].func));
            PanicNotZero(PioSetDir32Bank(bank, mask, 0));
            PanicNotZero(PioSet32Bank(bank, mask, mask));
            PanicNotZero(PioSetStrongBias32Bank(bank, mask, mask));
    }
}

void uart_data_stream_init(void)
{
    /* Assign task message handler */
    theUARTStreamTask.task.handler = UARTStreamMessageHandler;

    /* Configure uart settings */
    StreamUartConfigure(VM_UART_RATE_115K2, VM_UART_STOP_ONE, VM_UART_PARITY_NONE);

    /* Get the sink for the uart */
    theUARTStreamTask.uart_sink = StreamUartSink();
    PanicNull(theUARTStreamTask.uart_sink);

    /* Get the source for the uart */
    theUARTStreamTask.uart_source = StreamUartSource();
    PanicNull(theUARTStreamTask.uart_source);

    /* Register uart source with task */
    VmalMessageSinkTask(StreamSinkFromSource(theUARTStreamTask.uart_source),
    &theUARTStreamTask.task);
}

void uart_data_stream_tx_data(const uint8 *data, uint16 length)
{
    uint16 offset = 0;
    uint8 *dest = NULL;

    /* Claim space in the sink, getting the offset to it */
    offset = SinkClaim(theUARTStreamTask.uart_sink, length);
    if(offset == 0xFFFF) Panic();

    /* Map the sink into memory space */
    dest = SinkMap(theUARTStreamTask.uart_sink);
    PanicNull(dest);

    /* Copy data into the claimed space */
    memcpy(dest+offset, data, length);

    /* Flush the data out to the uart */
    PanicZero(SinkFlush(theUARTStreamTask.uart_sink, length));
}

void uart_data_stream_rx_data(Source src)
{
    uint16 length;
    const uint8 *data;

    while ((length = SourceBoundary(src)) > 0) {
        data = SourceMap(src);
        PanicNull((void*)data);

        // 将数据追加到行缓冲区
        uint16 i;
        for (i = 0; i < length; i++) {
            char ch = data[i];
            if (ch == '\n' || ch == '\r') {
                // 遇到换行符，解析当前行
                if (theUARTStreamTask.line_len > 0) {
                    theUARTStreamTask.line[theUARTStreamTask.line_len] = '\0';
                    parse_and_handle_line(theUARTStreamTask.line, theUARTStreamTask.line_len);
                    theUARTStreamTask.line_len = 0;
                }
            } else {
                // 普通字符，存入缓冲区（防止溢出）
                if (theUARTStreamTask.line_len < RX_LINE_BUFFER_SIZE - 1) {
                    theUARTStreamTask.line[theUARTStreamTask.line_len++] = ch;
                } else {
                    // 缓冲区满，丢弃该行（或可复位）
                    theUARTStreamTask.line_len = 0;
                }
            }
        }

        // 丢弃已处理的数据
        SourceDrop(src, length);
    }
}

void UARTStreamMessageHandler (Task pTask, MessageId pId, Message pMessage)
{
    UNUSED(pTask);
    switch (pId)
    {
        case MESSAGE_MORE_DATA:
        uart_data_stream_rx_data(((MessageMoreData *)pMessage)->source);
        break;
        default:
        break;
    }
}