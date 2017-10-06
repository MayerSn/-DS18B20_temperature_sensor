#include "main.h"
#include "init.h"
//#include "stm32f10x_gpio.h"
#include "usart_file.h"
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include <string.h>
extern uint8_t fl, convert_read_flag;
int main(void){
  uint8_t tte[3];
    init_all();
   
   
    while(1){
       
        convert_read_flag=1;
        send_reset ();
        //while (fl==0);
        fl=0;
        for(int i=0; i<200000;i++);
        send_reset ();
        
        for(int i=0; i<1000000;i++);
        
    }
}
