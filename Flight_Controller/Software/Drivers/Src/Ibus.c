/*
 * Ibus.c
 *
 *  Created on: 12 déc. 2020
 *      Author: Théo
 */
#include "../Inc/Ibus.h"

typedef enum rx_cplt_mode_e{
	MODE_CPLT_HALF,
	MODE_CPLT_FULL
}rx_cplt_mode_e;

static rx_cplt_mode_e rx_cplt_mode ;
static ibus_t * ibus ;	//On ne peut utilsier qu'une radio en ibus à la fois

//Fonction privé qui test si le checsum est bon sur un buffer
uint32_t check_sum_f(uint8_t * buffer);
uint32_t check_sum_f(uint8_t * buffer){
	uint32_t sum = 0 ;
	for(uint8_t b=0; b <30; b++)
		sum += (uint32_t)buffer[b];
	return sum ;
}

void IBUS_init(ibus_t * ibus_, UART_HandleTypeDef * huart, void(*data_rdy_callback)){
	ibus = ibus_;
	ibus->huart = huart ;
	ibus->data_rdy_callback = data_rdy_callback ;
	//On lance la lecture
	HAL_UART_Receive_DMA(ibus->huart, ibus->dma_buffer, SIZE_DMA_BUFFER);
	ibus->index_write = 1 ;
}

void IBUS_RxCplt(UART_HandleTypeDef * huart){
	if(huart == ibus->huart){
		rx_cplt_mode = MODE_CPLT_FULL;
		ibus->data_rdy_callback();
	}
}
void IBUS_RxHalfCplt(UART_HandleTypeDef * huart){
	if(huart == ibus->huart){
		rx_cplt_mode = MODE_CPLT_HALF ;
		ibus->data_rdy_callback();
	}
}

bool_e IBUS_Channel_Update(void){

	bool_e to_return = FALSE ;

	if(rx_cplt_mode == MODE_CPLT_HALF)
	{
		for(uint8_t c = 0; c < SIZE_DMA_BUFFER / 2; c++){
			switch(ibus->state){
				case STATE_IBUS_LENGTH :
					//ON sait que si on reçoit pas une lentgh de 0x20 (32) c pas bon
					if(ibus->dma_buffer[c] == 0x20){
						ibus->state = STATE_IBUS_COMMAND ;
						ibus->buffer[0] = ibus->dma_buffer[c] ;
					}
					break;
				case STATE_IBUS_COMMAND :
					//Seul commande qu'on connait btw
					if(ibus->dma_buffer[c] == 0x40){
						ibus->state = STATE_IBUS_CHANNELS ;
						ibus->buffer[1] = ibus->dma_buffer[c] ;
						ibus->compteur = 2 ;
					}
					//ON check que du coup c pas la commande de départ au cas ou et sinon retour au début
					else if(ibus->dma_buffer[c] != 0x20)
						ibus->state = STATE_IBUS_LENGTH ;
					break ;

				case STATE_IBUS_CHANNELS :
					ibus->buffer[ibus->compteur++] = ibus->dma_buffer[c] ;
					if(ibus->compteur == 32){
						uint32_t cs = (uint32_t)(ibus->buffer[30] | (ibus->buffer[31] << 8)) ;
						if(0xFFFF - check_sum_f(ibus->buffer)== cs)
						{
							for(uint32_t radio = 0; radio < 28; radio +=2)
								ibus->channels[radio / 2] = (int32_t)(ibus->buffer[radio+2] | (ibus->buffer[radio + 3] << 8));
							to_return = TRUE;
						}
						ibus->state = STATE_IBUS_LENGTH ;
					}
					break;
			}
		}
	}
	else
	{
		for(uint8_t c = 10; c < SIZE_DMA_BUFFER ; c++){
			switch(ibus->state){
				case STATE_IBUS_LENGTH :
					//ON sait que si on reçoit pas une lentgh de 0x20 (32) c pas bon
					if(ibus->dma_buffer[c] == 0x20){
						ibus->state = STATE_IBUS_COMMAND ;
						ibus->buffer[0] = ibus->dma_buffer[c] ;
					}
					break;
				case STATE_IBUS_COMMAND :
					//Seul commande qu'on connait btw
					if(ibus->dma_buffer[c] == 0x40){
						ibus->state = STATE_IBUS_CHANNELS ;
						ibus->buffer[1] = ibus->dma_buffer[c] ;
						ibus->compteur = 2 ;
					}
					//ON check que du coup c pas la commande de départ au cas ou et sinon retour au début
					else if(ibus->dma_buffer[c] != 0x20)
						ibus->state = STATE_IBUS_LENGTH ;
					break ;

				case STATE_IBUS_CHANNELS :
					ibus->buffer[ibus->compteur++] = ibus->dma_buffer[c] ;
					if(ibus->compteur == 32){
						uint32_t cs = (uint32_t)(ibus->buffer[30] | (ibus->buffer[31] << 8)) ;
						if(0xFFFF - check_sum_f(ibus->buffer)== cs)
						{
							for(uint32_t radio = 0; radio < 28; radio +=2)
								ibus->channels[radio / 2] = (int32_t)(ibus->buffer[radio+2] | (ibus->buffer[radio + 3] << 8));
							to_return = TRUE;
						}
						ibus->state = STATE_IBUS_LENGTH ;
					}
					break;
			}
		}
	}



	return to_return ;
}


