#ifndef __USART_F_H
#define __USART_F_H
void init_usart();
void send_convert_commands();
void send_reset();
void DMA1_Channel6_IRQHandler();
#endif
