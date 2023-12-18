/*
 * dma_ring_buffer.hpp
 *
 *  Created on: Nov 1, 2023
 *      Author: tunah
 */

#ifndef DMA_RING_BUFFER_HPP_
#define DMA_RING_BUFFER_HPP_

#include "stdint.h"
#include "stddef.h"
#include "vector"
#include "usart.h"
#include "queue"

#define DMA_BUFFER 1024


using namespace std;

class dma_ring_buffer
{
public:
	dma_ring_buffer(UART_HandleTypeDef *p_hdma_st, size_t dma_buf_size = DMA_BUFFER);
	void veri_al();
	queue<uint8_t> data_qu;

private:

	size_t dma_buf_size_u32;
	UART_HandleTypeDef *uart_dma_st;
	uint32_t tail_u32;
	uint8_t p_buffer[20];

	bool ilk_calisma_u1;
};


#endif /* DMA_RING_BUFFER_HPP_ */
