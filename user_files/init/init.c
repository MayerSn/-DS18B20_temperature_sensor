#include "init.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "usart_file.h"

void init_all(){
    //Led init
   // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);
//    GPIO_InitTypeDef gpioIS;
//    gpioIS.GPIO_Mode = GPIO_Mode_AF_PP;
//    gpioIS.GPIO_Pin = GPIO_Pin_5;
//    gpioIS.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOD,&gpioIS);
    
//    //Timer init
//    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2, ENABLE);
    
//    TIM_TimeBaseInitTypeDef TIME_Str;
//    TIME_Str.TIM_Prescaler = (uint16_t)(7200); //10000 ticks per sec
//    TIME_Str.TIM_Period = 10000;
//    TIM_TimeBaseInit(TIM2,&TIME_Str);
//    TIM_ITConfig (TIM2, TIM_IT_Update, ENABLE);
//    NVIC_EnableIRQ (TIM2_IRQn);
//    TIM_Cmd (TIM2,ENABLE);
  
    //USART INIT 
   init_usart ();
   
     
}



//////////////////////////
/// Temp sensor 1820 
///////////////////////////////

//One wire pin toggle

void delay_nus(vu32 nCount)
{
	
//	u16 TIMCounter = nCount;
	
// TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
// RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
// TIM_TimeBaseStructure.TIM_Period = 1;                 //?O????OO
// TIM_TimeBaseStructure.TIM_Prescaler = 72;       //72M?O??AE??1MHz
// TIM_TimeBaseStructure.TIM_ClockDivision = 0;  
// TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //IoIA??Ey
// TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
//  TIM_Cmd(TIM2, ENABLE);
//  TIM_SetCounter(TIM2, TIMCounter);
//  while (TIMCounter)
//  {
//    TIMCounter = TIM_GetCounter(TIM2);
//  }
//  TIM_Cmd(TIM2, DISABLE);
}
/////////////////////////////////////////////////////////////////
#define DS18B20_BIT_OUT        GPIO_Pin_5
#define DS18B20_PORT_OUT        GPIOD

#define DS18B20_BIT_IN        GPIO_Pin_6
#define DS18B20_PORT_IN        GPIOD

#define READ_DS18B20()        GPIO_ReadInputDataBit(DS18B20_PORT_OUT, DS18B20_BIT_OUT)
#define CLR_OP_1WIRE()        GPIO_ResetBits(DS18B20_PORT_OUT, DS18B20_BIT_OUT)
#define SET_OP_1WIRE()        GPIO_SetBits(DS18B20_PORT_OUT, DS18B20_BIT_OUT)
#define CHECK_IP_1WIRE()    GPIO_ReadInputDataBit(DS18B20_PORT_IN, DS18B20_BIT_IN)

void   DIR_1WIRE_IN() 
{//DS18B20_PORT->CRH &= 0xFFFFFF0F;GPIOC->CRH |= 0x00000040;
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin =  DS18B20_BIT_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING;
    GPIO_Init(DS18B20_PORT_IN, &GPIO_InitStructure);
}
		
void    DIR_1WIRE_OUT()        
{//DS18B20_PORT->CRH &= 0xFFFFFF0F;GPIOC->CRH |= 0x00000030;
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin =  DS18B20_BIT_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_AF_PP;
    GPIO_Init(DS18B20_PORT_OUT, &GPIO_InitStructure);		
}
		
void write_1820(u8 x)  //INVERT
{    int16_t i;  
   
        for(i=0; i<8; i++)  
    {  
               CLR_OP_1WIRE();//SET_OP_1WIRE();//
								
               if(x & 0x01)    //??Ey?YAE??IE???II??A??
                       SET_OP_1WIRE();  //CLR_OP_1WIRE();//
               else  
                       CLR_OP_1WIRE(); //SET_OP_1WIRE();// 
                x>>=1;
               delay_nus(25);   //15~60us  
              SET_OP_1WIRE(); // CLR_OP_1WIRE();// 
							 delay_nus(10);
    }  
    SET_OP_1WIRE();  //CLR_OP_1WIRE();//
}  
u8 read_1820(void)  //INVERT  DIR_WIRE is commented out
{       int16_t i;
        u8 x = 0;
       
        //DIR_1WIRE_OUT();
    for(i=0; i<8; i++)  
    {  
            CLR_OP_1WIRE();//SET_OP_1WIRE();//  
            x >>= 1;
                SET_OP_1WIRE(); //CLR_OP_1WIRE();// 
           // DIR_1WIRE_IN();
            //?AEy?Y,?O?II???E?  
               
            if(CHECK_IP_1WIRE())  
                    x |= 0x80;
            delay_nus(60); //60~120us      
           // DIR_1WIRE_OUT();  
    }
    return (x);  
}

void init_1820()  //INVERT BEFORE FINAL DIR_WIRE is commented out
{  
   // DIR_1WIRE_OUT();
    SET_OP_1WIRE();//CLR_OP_1WIRE();  
		//delay_nus(100);  
    CLR_OP_1WIRE();  //SET_OP_1WIRE();//
    delay_nus(450);  //446//480usOOEI  
    SET_OP_1WIRE();//CLR_OP_1WIRE();//  
    //DIR_1WIRE_IN();  
    //delay_nus(65);//65     //15~60us  
    //while(!CHECK_IP_1WIRE()); 
	//	delay_nus(420);
	//	SET_OP_1WIRE();//CLR_OP_1WIRE(); 
    //DIR_1WIRE_OUT();  
 //  SET_OP_1WIRE();  // CLR_OP_1WIRE();
    delay_nus(210);   //60~240us  
}

void DS18B20_Convert(void)                //IA?E????
{
     init_1820();        //??I?18b20  
     write_1820(0xcc);   // ???o????AuAi 
      //delay_nus (200);
     write_1820(0x44);
}

void DS18B20_Convert_start(void)                //IA?E????
{
     init_1820();        //??I?18b20  
     //write_1820(0xcc);   // ???o????AuAi  
     //write_1820(0x44);
}
void DS18B20_Convert_end(void)                //IA?E????
{
     //init_1820();        //??I?18b20  
     write_1820(0xcc);   // ???o????AuAi  
     write_1820(0x44);
}
	
int16_t DS18B20_Read(void)                   //?AE?IA?EO?
{   int16_t res;
		int16_t res_fin;
    init_1820();  
    write_1820(0xcc);  //???o?AAuAi  
   
    write_1820(0xbe); 
//delay_nus(5);	
    res=read_1820();  //?AEy?Y  
    res+=(int16_t)read_1820()<<8; 
    return res;
}

//int16_t DS18B20_Power_Suply(void)                   //?AE?IA?EO?
//{  
//    int16_t res;
//    init_1820();  
//	return res;
//    write_1820(0xcc);  //???o?AAuAi  
//    write_1820(0xb4);  
//    res=read_1820();  //?AEy?Y  
//	send_to_uart(res); 
//    res+=(u16)read_1820()<<8; 
//	send_to_uart(res>>8); 
//    return res;
//}

//uint8_t flag=0;
//void TIM2_IRQHandler(){
//    TIM_ClearITPendingBit (TIM2,TIM_IT_Update);
//    if(flag==0){
//        //GPIO_SetBits(GPIOD,GPIO_Pin_5);
//        flag=1;
//    }
//    else{
//        flag=0;
//        // GPIO_ResetBits(GPIOD,GPIO_Pin_5);
//    }
//}
