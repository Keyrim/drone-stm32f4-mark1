/*
 * Telemetry.c
 *
 *  Created on: Dec 18, 2020
 *      Author: Théo
 */

#include "Telemetry.h"
#include "../Data_Logger/Data_logger.h"

#define MAX_TRANSMIT_SPEED 	3000						//En octets par secondes, 2000 permet de ne pas saturer le module radio
#define PERIODE_1_OCTET	(1000000 / MAX_TRANSMIT_SPEED)	//Periode alouée à l'envoit d'un octet

static telemetry_t * telem ;	//We dont use multiples telemetry anyways

//Private function to know the number of bytes available
uint16_t get_nb_bytes_available(telemetry_t * telem);
uint16_t get_nb_bytes_available(telemetry_t * telem){
	uint16_t nb = 0 ;
	if(telem->index_write_rx > telem->index_read_rx){
		nb = telem->index_write_rx - telem->index_read_rx ;
	}
	return nb;
}

void TELEMETRY_Init(telemetry_t * telem_, UART_HandleTypeDef * huart)
{
	telem = telem_ ;
	telem->huart = huart ;

	//On envoit un premier message qui sert sutout à éviter que les premiers octet utiles soient perdu
	static uint8_t hey_msg[] = "Hey it s me Theo...\n";
	HAL_UART_Transmit_DMA(telem->huart, hey_msg, 20);

	//Activation de l'it "idle" qui préviendra de la fin de la récéption d'un message
	__HAL_UART_ENABLE_IT(telem->huart, UART_IT_IDLE);
	//On lance la récéption en DMA
	HAL_UART_Receive_DMA(telem->huart, telem->buffer_rx, LEN_RX_BUFFER);
}


void TELEMETRY_Process(uint32_t time_us){

	//	--------------------	Transmission	------------------------
	//Ce qui suit vide le buffer en transmettant on contenu en respectant la limite
	//de vitesse maximum
	if(telem->index_write_tx != telem->index_read_tx && time_us > telem->next_send)
	{
		telem->next_send = time_us + telem->len_portion_tx[telem->index_read_tx] * PERIODE_1_OCTET ;

		//On transmet les données
		HAL_UART_Transmit_DMA(telem->huart, &telem->buffer_tx[SIZE_PORTION_BUFFER*telem->index_read_tx], telem->len_portion_tx[telem->index_read_tx]);
		//Incrémentation de l'index
		telem->index_read_tx ++ ;
		telem->index_read_tx = telem->index_read_tx % NB_PORTION_BUFFER ;
	}

	//	---------------------	Reception	-------------------------------

	uint16_t bytes_avaibles = get_nb_bytes_available(telem);
	if(bytes_avaibles > 2)	//Minimum trois octets pour faire quoi que ce soit
	{
		uint16_t bytes_used = 1 ;
		if(telem->buffer_rx[telem->index_read_rx] == '$')	//Détection de début de séquence
		{
			//Number of byte recquiered including the '$' and the checksum to use the current frame we are on
			data_t * current_data = DATA_LOGGER_Get_Data(telem->buffer_rx[telem->index_read_rx+1]);
			uint16_t bytes_recquired = (current_data->format / 20) + 3 ;

			if(bytes_recquired > bytes_avaibles)
				return ;	//We just wait for then next call with hopefully more data available
			bytes_used = bytes_recquired;

			//Len is id(1 byte) + data(x bytes)
			uint16_t len_frame = bytes_recquired - 2 ;

			//Checksum
			uint16_t checksum = 0 ;
			for(uint16_t b = 0; b < len_frame; b++)
			{
				checksum += telem->buffer_rx[telem->index_read_rx+1+b];
			}
			if(checksum % 256 == telem->buffer_rx[telem->index_read_rx + bytes_recquired-1])
			{
				DATA_LOGGER_Reception(&telem->buffer_rx[telem->index_read_rx+1]);
			}
		}
		telem->index_read_rx += bytes_used ;
	}

}


void TELEMETRY_Send_Data(uint8_t * data, uint8_t len){
	uint8_t to_send[SIZE_PORTION_BUFFER];
	uint32_t checksum = 0 ;


	to_send[0] = '$';

	for(uint8_t b = 0; b < len; b++){
		to_send[1 + b] = data[b];
		checksum += (uint32_t)to_send[1 + b];
	}

	to_send[1 + len] = (uint8_t)(checksum % 256) ;
	TELEMETRY_Send_Data_Raw(to_send, len+2);
}

void TELEMETRY_Send_Data_Raw(uint8_t * data, uint8_t len){

	//Copie du tableau dans le buffer
	for(uint8_t c = 0; c < len; c++)
		telem->buffer_tx[telem->index_write_tx*SIZE_PORTION_BUFFER + c] = data[c];
	//On enregistre la taille de la donnée
	telem->len_portion_tx[telem->index_write_tx] = len ;

	//Incrémentation de l'index
	telem->index_write_tx ++ ;
	telem->index_write_tx = telem->index_write_tx % NB_PORTION_BUFFER ;
}

void TELEMETRY_Idle_Line_Detection(void){
	//On met à jour l'index d'écriture du buffer rx
	telem->index_write_rx = LEN_RX_BUFFER - telem->huart->hdmarx->Instance->NDTR ;
}


