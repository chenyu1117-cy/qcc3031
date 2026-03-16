#ifndef __UART_H_
#define __UART_H_

#include <sink.h>
#include <source.h>
#include <message.h>

#define RX_LINE_BUFFER_SIZE 256   // 添加缓冲区大小定义

typedef struct
{
    TaskData task;
    Sink uart_sink;
    Source uart_source;
    char line[RX_LINE_BUFFER_SIZE];   // 行缓冲区
    uint16 line_len;                   // 当前已接收字节数
} UARTStreamTaskData;

extern UARTStreamTaskData theUARTStreamTask;

void uart_device_init(void);
void UARTStreamMessageHandler (Task pTask, MessageId pId, Message pMessage);
void uart_data_stream_rx_data(Source src);
void uart_data_stream_tx_data(const uint8 *data, uint16 length);
void uart_data_stream_init(void);
void uart_pios_init(void);

#endif /* __UART_H_ */