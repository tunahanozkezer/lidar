/*
 * dma_ring_buffer.cpp
 *
 *  Created on: Nov 1, 2023
 *      Author: tunah
 */




#include "dma_ring_buffer.hpp"
#include "iostream"

dma_ring_buffer::dma_ring_buffer(UART_HandleTypeDef *p_hdma_st, size_t dma_buf_size)
{
	dma_buf_size_u32 = dma_buf_size;
	uart_dma_st      = p_hdma_st;

	ilk_calisma_u1 = true;
}


queue<uint8_t> dma_ring_buffer::veri_al()
{
	const  uint32_t head_u32 = dma_buf_size_u32 - uart_dma_st->hdmarx->Instance->NDTR;
	static uint8_t yazilan_veri = 0;

	if(true == ilk_calisma_u1)
	{
		HAL_UART_Receive_DMA(uart_dma_st, p_buffer, dma_buf_size_u32);
		ilk_calisma_u1 = false;
	}

	if(head_u32 > tail_u32 )
	{
		for(size_t i = tail_u32; i< head_u32; i++)
		{
			yazilan_veri = uart_dma_st->pRxBuffPtr[i];
			data_qu.push(yazilan_veri);
		}

		tail_u32 = head_u32;
	}
	else if (head_u32 < tail_u32)
	{
		for(size_t i = tail_u32; i< dma_buf_size_u32; i++)
		{
			data_qu.push(uart_dma_st->pRxBuffPtr[i]);
		}


		for(size_t i = 0; i< head_u32; i++)
		{
			data_qu.push(uart_dma_st->pRxBuffPtr[i]);
		}

		tail_u32 = head_u32;

	}
//	return data_qu;
}
