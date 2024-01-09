/*
 * dma_ring_buffer.hpp
 *
 *  Created on: Nov 1, 2023
 *      Author: tunah
 */

#ifndef DMA_RING_BUFFER_HPP_
#define DMA_RING_BUFFER_HPP_

#include "usart.h"
#include <vector>
#include <memory>

#define DMA_BUFFER 1024

using namespace std;

class dma_ring_buffer
{
public:
	dma_ring_buffer(std::shared_ptr<UART_HandleTypeDef> p_hdma_st, size_t dma_buf_size = DMA_BUFFER);
	 std::vector<uint8_t> get_data();

private:

	std::unique_ptr<uint8_t[]> p_buffer;
	size_t dma_buf_size_u32;
	std::shared_ptr<UART_HandleTypeDef> uart_handle;
	uint32_t tail_u32;
};


#endif /* DMA_RING_BUFFER_HPP_ */
