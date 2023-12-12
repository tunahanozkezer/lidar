/*
 * tasks.cpp
 *
 *  Created on: Nov 7, 2023
 *      Author: tunah
 */


#include "tasks.hpp"
#include "usart.h"
#include "gpio.h"
#include "dma_ring_buffer.hpp"
#include "tf_luna.hpp"

uint8_t tx_buff[]={0,1,2,3,4,5,6,7,8,9};


dma_ring_buffer uart_1_buffer(&huart1, 20);
dma_ring_buffer uart_2_buffer(&huart2, 20);
uint8_t cikilan_veri_u8[20];

tf_luna luna_ct(5);

rtos_task comm_task_init(tasks::comm_task, "COMM_Task");

//queue<uint8_t> veriler;
uint8_t data_aa[255];
uint8_t sifirla = 0;
static int a = 0;
void tasks::comm_task( void * pvParameters)
{
	 const TickType_t xFrequency = 100;
	 uart_veri veri = {0};
	 luna_ct.version_downward();

	for(;;)
	{
		 uart_1_buffer.veri_al();
		 uart_2_buffer.veri_al();
		 luna_ct.parse_byte(uart_2_buffer.data_qu);

		 while(uart_1_buffer.data_qu.empty() == false)
		 {
			 data_aa[a] = uart_1_buffer.data_qu.front();
			 uart_1_buffer.data_qu.pop();
			 a++;
		 }
		 if(sifirla == 1)
		 {
			 a = 0;
			 sifirla = 0;
		 }
// 		 if(uart_2_buffer.data_qu.size() > 0)
// 		 {
// 			 static int a;
// 			 a++;
// 		 }
		HAL_GPIO_TogglePin(ob_led_GPIO_Port, ob_led_Pin);
		vTaskDelay( xFrequency );
	}
}
