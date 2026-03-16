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
    uint16 length = 0;
    const uint8 *data = NULL;

    /* Get the number of bytes in the specified source before the next packet
    boundary */
    if(!(length = SourceBoundary(src)))
    return;

    /* Maps the specified source into the address map */
    data = SourceMap(src);
    PanicNull((void*)data);

/*
    UART_DEBUG(("length:%d\n",length));
    UART_DEBUG_DUMP(data,length);
*/

    /* Check if received "hi" */
    if (length == 2 && data[0] == 'h' && data[1] == 'i') {
        uart_data_stream_tx_data((const uint8*)"hallo\n", 6);
    } else {
        uart_data_stream_tx_data(data, length);
    }

    /* Discards the specified amount of bytes from the front of the specified
    source */
    SourceDrop(src, length);
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
