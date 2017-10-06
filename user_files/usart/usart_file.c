#include "usart_file.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"
#include "misc.h"
#include "string.h"
#define DMA_BUFF_SIZE  32
uint8_t Rx_Buff[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t Tx_Conv_Buff[16] = {//0xCC
                            0x00,0x00,0xFF,0xFF, 0x00,0x00,0xFF,0xFF,
                            //0x44
                           0x00,0x00,0xFF,0x00,0x00,0x00,0xFF,0x00 };
uint8_t Tx_Buff[32]={ 
    //0xCC
    0x00,0x00,0xFF,0xFF, 0x00,0x00,0xFF,0xFF,
          //0xBE 
          0x00,0xFF,0xFF,0xFF, 0xFF,0xFF,0x00,0xFF,           
                     
                     //READ TEMPER
                     0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,
                                0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF};
GPIO_InitTypeDef gpio_IS;
DMA_InitTypeDef dmaIS;
USART_InitTypeDef USART_IS;
void init_usart(){
    
    
   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE);
    
   
///USART  DMA CONFIG  
    __enable_irq ();
    //TX 1-Wire
    gpio_IS.GPIO_Mode = GPIO_Mode_IN_FLOATING; //OD FOR SENSOR
    gpio_IS.GPIO_Pin = GPIO_Pin_5;
    gpio_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_PinRemapConfig (GPIO_Remap_USART2, ENABLE);
    GPIO_Init(GPIOD, &gpio_IS);
    
    //USART
    USART_IS.USART_BaudRate =9600;//312=115200; //3750 = 9600;//180650;
    USART_IS.USART_WordLength = USART_WordLength_8b;
    USART_IS.USART_StopBits = USART_StopBits_1;
    USART_IS.USART_Parity = USART_Parity_No;
    USART_IS.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_IS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_IS);
    USART_HalfDuplexCmd (USART2, ENABLE); 
   
    //DMA
    
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1, ENABLE);
   
    DMA_DeInit(DMA1_Channel7);
    //Tx
    dmaIS.DMA_BufferSize = DMA_BUFF_SIZE;
    dmaIS.DMA_DIR = DMA_DIR_PeripheralDST;
    dmaIS.DMA_M2M = DMA_M2M_Disable;
    dmaIS.DMA_MemoryBaseAddr = (uint32_t)Tx_Buff;
    dmaIS.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dmaIS.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dmaIS.DMA_Mode = DMA_Mode_Normal;
    dmaIS.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
    dmaIS.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaIS.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dmaIS.DMA_Priority = DMA_Priority_Low;
    DMA_Init(DMA1_Channel7,&dmaIS);
   
    //Rx
    dmaIS.DMA_BufferSize = DMA_BUFF_SIZE;
    dmaIS.DMA_DIR = DMA_DIR_PeripheralSRC;
    dmaIS.DMA_M2M = DMA_M2M_Disable;
    dmaIS.DMA_MemoryBaseAddr = (uint32_t)Rx_Buff;
    dmaIS.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dmaIS.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dmaIS.DMA_Mode = DMA_Mode_Normal;
    dmaIS.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
    dmaIS.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaIS.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dmaIS.DMA_Priority = DMA_Priority_Low;
    DMA_Init(DMA1_Channel6,&dmaIS);      
    
    DMA_ITConfig (DMA1_Channel6, DMA_IT_TC, ENABLE);
     
    NVIC_InitTypeDef nvicIS;
    nvicIS.NVIC_IRQChannel = DMA1_Channel6_IRQn;
    nvicIS.NVIC_IRQChannelCmd = ENABLE;
    nvicIS.NVIC_IRQChannelPreemptionPriority =2;
    nvicIS.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvicIS);
//    nvicIS.NVIC_IRQChannel = DMA1_Channel7_IRQn;
//    nvicIS.NVIC_IRQChannelCmd = ENABLE;
//    nvicIS.NVIC_IRQChannelPreemptionPriority =2;
//    nvicIS.NVIC_IRQChannelSubPriority = 0;
//    NVIC_Init(&nvicIS);
   
    //Switch all on
    USART_Cmd (USART2, ENABLE); 
    
    
}
uint8_t flag_reset_sent=0;
void send_reset(){

    //Disable peripherals
    USART_Cmd (USART2, DISABLE);
    DMA_Cmd (DMA1_Channel7, DISABLE);
    DMA_Cmd (DMA1_Channel6, DISABLE);
    while((USART2->CR1&2000));
    //Change Baudrate to 9600
    USART2->BRR =3750;     
    USART_Cmd (USART2, ENABLE);
    //Change memory base address and buffer size/counter   
    uint8_t reset = 0xF0;
    DMA1_Channel7->CMAR = (uint32_t)&reset;
    DMA1_Channel7->CNDTR = (uint16_t)1;
    DMA1_Channel6->CNDTR = (uint16_t)1;
    //Send
    flag_reset_sent=1;
    USART_DMACmd (USART2,  USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
    DMA_Cmd (DMA1_Channel6, ENABLE); 
    DMA_Cmd (DMA1_Channel7, ENABLE);
}

uint8_t fl=0, convert_read_flag=0;//conv=0, read=1

void send_convert_commands(){
    //Change usart baudrate to 115200
    while((USART2->CR1&0x2000));
    USART2->BRR =312; 
    USART_Cmd (USART2, ENABLE); 
    
    while(!(USART2->CR1&0x2000));
    //Change memBaseAddr for DMA_Tx
    DMA1_Channel7->CMAR = (uint32_t)&Tx_Conv_Buff;
    //Change data counter for both DMAs
    DMA1_Channel7->CNDTR = (uint16_t)16; // 0xcc and 0x44
    DMA1_Channel6->CNDTR = (uint16_t)16; // receive the same
    //Send
    USART_DMACmd (USART2,  USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
    DMA_Cmd (DMA1_Channel6, ENABLE); 
    DMA_Cmd (DMA1_Channel7, ENABLE);
      
}

void send_read_commands(){
    //fl=1;
    //Change usart baudrate to 115200
    while((USART2->CR1&0x2000));
    USART2->BRR =312; 
    USART_Cmd (USART2, ENABLE);
    while(!(USART2->CR1&0x2000));
     //memset (Rx_Buff,0, sizeof(Rx_Buff));
    //Change memBaseAddr for DMA_Tx
    DMA1_Channel7->CMAR = (uint32_t)&Tx_Buff;
    //Change data counter for both DMAs
    DMA1_Channel7->CNDTR = (uint16_t)32; // 
    DMA1_Channel6->CNDTR = (uint16_t)32; // receive the same
    //Send
    USART_DMACmd (USART2,  USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
    DMA_Cmd (DMA1_Channel6, ENABLE); 
    DMA_Cmd (DMA1_Channel7, ENABLE);
    
}




void DMA1_Channel6_IRQHandler(){
   
     DMA_Cmd (DMA1_Channel6, DISABLE);
     DMA_Cmd (DMA1_Channel7, DISABLE);
     USART_Cmd (USART2, DISABLE);
     
     DMA_ClearITPendingBit (DMA1_IT_TC6);
     DMA_ClearITPendingBit (DMA1_IT_TC7);
     
     DMA_ClearITPendingBit (DMA1_IT_HT6);
     DMA_ClearITPendingBit (DMA1_IT_HT7);
     
     DMA_ClearFlag (DMA1_FLAG_TC7);
     DMA_ClearFlag (DMA1_FLAG_TC6);
     
     DMA_ClearFlag (DMA1_FLAG_HT7);
     DMA_ClearFlag (DMA1_FLAG_HT6);
     
     USART_ClearFlag (USART2, USART_FLAG_TC);
     USART_ClearITPendingBit (USART2, USART_IT_TC);
            
         
    
         
     if(convert_read_flag==1){
        convert_read_flag=2;
        send_convert_commands ();
     }
     if(convert_read_flag==2){
        convert_read_flag=0;
        send_read_commands (); 
     }
  
     

}
