/*
 * dma_ring_buffer.cpp
 *
 *  Created on: Nov 1, 2023
 *      Author: tunah
 */




#include "dma_ring_buffer.hpp"
#include "iostream"

dma_ring_buffer::dma_ring_buffer(UART_HandleTypeDef *p_hdma_st, size_t dma_buf_size): p_buffer{new uint8_t[dma_buf_size]}, dma_buf_size_u32 {dma_buf_size}, uart_handle{ p_hdma_st}
{

	HAL_UART_Receive_DMA(uart_handle, p_buffer.get(), dma_buf_size_u32);
}


std::vector<uint8_t> dma_ring_buffer::get_data()
{
	const uint32_t head_u32 = dma_buf_size_u32 - uart_handle->hdmarx->Instance->NDTR;
	std::vector<uint8_t> received_data;

	if (head_u32 > tail_u32)
	{
		for (size_t i = tail_u32; i < head_u32; i++)
		{
			received_data.push_back(uart_handle->pRxBuffPtr[i]);
		}

		tail_u32 = head_u32;
	}
	else if (head_u32 < tail_u32)
	{
		for (size_t i = tail_u32; i < dma_buf_size_u32; i++)
		{
			received_data.push_back(uart_handle->pRxBuffPtr[i]);
		}

		for (size_t i = 0; i < head_u32; i++)
		{
			received_data.push_back(uart_handle->pRxBuffPtr[i]);
		}


		tail_u32 = head_u32;
	}

	return received_data;
}
