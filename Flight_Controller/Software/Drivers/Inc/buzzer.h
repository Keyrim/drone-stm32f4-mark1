/*
 * buzzer.h
 *
 *  Created on: Feb 27, 2021
 *      Author: Théo
 */

#ifndef IHM_BUZZER_H_
#define IHM_BUZZER_H_

#include "stm32f4xx_hal.h"
#include "../../Core/OS/macro_types.h"

#define SOL_d2 104
#define LA2    110
#define LA_d2  114
#define SI2    124
#define DO3    131
#define DO_d3  139
#define RE3    147
#define RE_d3  156
#define MI3    165
#define FA3    175
#define FA_d3  185
#define SOL3   196
#define SOL_d3 208
#define LA3    220
#define LA_d3  233
#define SI3    247
#define DO4    262
#define DO_d4  277
#define RE4    294
#define RE_d4  311
#define MI4    330
#define FA4    349
#define FA_d4  370
#define SOL4   392
#define SOL_d4 415
#define LA4    440
#define LA_d4  466
#define SI4    494
#define DO5    523
#define DO_d5  554
#define RE5    587
#define RE_d5  622
#define MI5    659
#define FA5    698
#define FA_d5  740
#define SOL5   784
#define SOL_d5 831
#define LA5    880
#define LA_d5  932
#define SI5    988
#define DO6    1046
#define DO_d6  1109
#define RE6    1175
#define RE_d6  1245
#define MI6    1319
#define FA6    1397
#define FA_d6  1480
#define SOL6   1568
#define SOL_d6 1661
#define LA6   1760
#define LA_d6  1865

typedef enum sequencies_e{
	SEQUENCE_NOTES_1,
	SEQUENCE_NOTES_COUNT
}sequencies_e;

typedef struct buzzer_t{
	TIM_HandleTypeDef * htim;
	HAL_StatusTypeDef hal_state ;
	uint8_t current_seq ;
	uint8_t index;
	uint16_t counter;
}buzzer_t;


void BUZZER_Init(buzzer_t * buzzer, TIM_HandleTypeDef * htim);
void BUZZER_Start(buzzer_t * buzzer, sequencies_e sequence);
void BUZZER_Stop(buzzer_t * buzzer);
void BUZZER_Update(buzzer_t * buzzer);

#endif /* IHM_BUZZER_H_ */
