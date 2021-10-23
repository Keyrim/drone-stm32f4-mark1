/*
 * mpu.c
 *
 *  Created on: 26 nov. 2020
 *      Author: Théo
 */

#include "mpu.h"
#include "../OS/time.h"



//Ces trois tableaux sont liés et permettent de savoir pour un SPI donné, quelle est le mpu associé
static SPI_HandleTypeDef * SPI_handlers[NB_MPU_USED] ;


static mpu_t * mpu_handlers[NB_MPU_USED];
static uint8_t mpu_init_compteur = 0 ;

//Private function prototypes
static void MPU_dma_transmit_complete(mpu_t * mpu);
static void MPU_dma_transmit_half_complete(mpu_t * mpu);

static void convert_gyro(mpu_t * mpu);
static void convert_acc(mpu_t * mpu);

static void MPU_cs_lock(mpu_t * mpu);
static void MPU_cs_unlock(mpu_t * mpu);

static void MPU_spi_slow(mpu_t * mpu);
static void MPU_spi_fast(mpu_t * mpu);

//Private functions def
static void convert_gyro(mpu_t * mpu){
	mpu->gyro_raw[GYRO_AXE_X] = -(int16_t)(mpu->gyro_data[1] << 8 | mpu->gyro_data[0]);
	mpu->gyro_raw[GYRO_AXE_Y] = (int16_t)(mpu->gyro_data[3] << 8 | mpu->gyro_data[2]);
	mpu->gyro_raw[GYRO_AXE_Z] = (int16_t)(mpu->gyro_data[5] << 8 | mpu->gyro_data[4]);

	//Convertion
	mpu->gyro[GYRO_AXE_X] = (float)mpu->gyro_raw[GYRO_AXE_X] * mpu->gyro_sensi ;
	mpu->gyro[GYRO_AXE_Y] = (float)mpu->gyro_raw[GYRO_AXE_Y] * mpu->gyro_sensi ;
	mpu->gyro[GYRO_AXE_Z] = (float)mpu->gyro_raw[GYRO_AXE_Z] * mpu->gyro_sensi ;
}
static void convert_acc(mpu_t * mpu){
	mpu->acc_raw[ACC_AXE_X] = -(int16_t)(mpu->acc_data[1] << 8 | mpu->acc_data[0]);
	mpu->acc_raw[ACC_AXE_Y] = (int16_t)(mpu->acc_data[3] << 8 | mpu->acc_data[2]);
	mpu->acc_raw[ACC_AXE_Z] = (int16_t)(mpu->acc_data[5] << 8 | mpu->acc_data[4]);

	//Convertion
	mpu->acc[ACC_AXE_X] = (float)mpu->acc_raw[ACC_AXE_X] * mpu->acc_sensi ;
	mpu->acc[ACC_AXE_Y] = (float)mpu->acc_raw[ACC_AXE_Y] * mpu->acc_sensi ;
	mpu->acc[ACC_AXE_Z] = (float)mpu->acc_raw[ACC_AXE_Z] * mpu->acc_sensi ;
}

static void MPU_cs_lock(mpu_t * mpu){
	HAL_GPIO_WritePin(mpu->pin_cs_gpio, mpu->pin_cs, RESET);
}
static void MPU_cs_unlock(mpu_t * mpu){
	HAL_GPIO_WritePin(mpu->pin_cs_gpio, mpu->pin_cs, SET);
}

static void MPU_spi_slow(mpu_t * mpu){
	mpu->spi_speed = MPU_SPI_SLOW;
	mpu->hspi->Init.BaudRatePrescaler = 64 ;
	mpu->hal_state = HAL_SPI_Init(mpu->hspi);
	HAL_Delay(10);
}
static void MPU_spi_fast(mpu_t * mpu){
	mpu->spi_speed = MPU_SPI_FAST;
	mpu->hspi->Init.BaudRatePrescaler = 4 ;
	mpu->hal_state = HAL_SPI_Init(mpu->hspi);
	HAL_Delay(10);
}


/*
 *
 */
sensor_request_e MPU_init(mpu_t * mpu, SPI_HandleTypeDef * hspi, GPIO_TypeDef * pin_cs_gpio, uint16_t pin_cs){


	//Requiert un SPI pour fonctionner
	if(hspi == NULL)
		return SENSOR_REQUEST_FAIL ;


	//CS pin must be high by default
	MPU_cs_unlock(mpu);
	HAL_Delay(1);
	TIME_delay_us_blocking(1);

	//Etat par default
	mpu->state = SENSOR_NOT_INIT ;

	//SPI slow pdt l'init, max si 1Mhz for the mpu
	if(mpu->spi_speed ==  MPU_SPI_FAST)
		MPU_spi_slow(mpu);


	//Définit d'après la doc pour les registres du mpu
	mpu->gyro_data = &mpu->data[8] ;
	mpu->acc_data = &mpu->data[0] ;


	//Application des paramètres de connexion
	mpu->hspi = hspi ;
	mpu->pin_cs = pin_cs ;
	mpu->pin_cs_gpio = pin_cs_gpio ;

	mpu->adresse = MPU6050_I2C_ADDR ;

	mpu_handlers[mpu_init_compteur] = mpu ;
	SPI_handlers[mpu_init_compteur++] = hspi ;

	//Wakeup mpu
	uint8_t wakeup_data[] = {MPU6050_PWR_MGMT_1, 0x80} ;
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_Transmit(mpu->hspi, wakeup_data, 2, 2);
	MPU_cs_unlock(mpu);

	HAL_Delay(150);

	//Clock select
	uint8_t mpu_clock_sel_pll_gyroz[] = {MPU6050_PWR_MGMT_1, 0x03};
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_Transmit(mpu->hspi, mpu_clock_sel_pll_gyroz, 2, 2);
	MPU_cs_unlock(mpu);

	HAL_Delay(15);

	//Disable I2C
	uint8_t mpu_disable_i2c[] = {MPU6050_USER_CTRL, 0x10};
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_Transmit(mpu->hspi, mpu_disable_i2c, 2, 2);
	MPU_cs_unlock(mpu);

	TIME_delay_us_blocking(2);

	//Digital low pass filter config
//	uint8_t set_dlpf[] = {MPU6050_CONFIG, 0x00};
//	MPU_cs_lock(mpu);
//	mpu->hal_state = HAL_SPI_Transmit(mpu->hspi, set_dlpf, 2, 2);
//	MPU_cs_unlock(mpu);

	//Pour l'init on ne s'occupe pas des cas HAL busy etc, c'est bon ou c'est pas bon c est tout^^
	if(mpu->hal_state != HAL_OK){
		mpu->state = SENSOR_ERROR ;
		return SENSOR_REQUEST_FAIL ;
	}

	mpu->state = SENSOR_IDDLE ;
	return SENSOR_REQUEST_OK;
}

/*
 *
 */
sensor_request_e MPU_init_gyro(mpu_t * mpu, MPU_gyro_range_e gyro_range, void (*gyro_data_callback)){

	mpu->gyro_range = gyro_range ;
	mpu->gyro_data_callback = gyro_data_callback ;

	//Si mpu non utilisable
	if(mpu->state != SENSOR_IDDLE)
		return SENSOR_REQUEST_FAIL ;

	if(mpu->spi_speed ==  MPU_SPI_FAST)
		MPU_spi_slow(mpu);

	//Configuration de la sensi du gyro dans le mpu
	//On lit
	uint8_t temp[2] ;
	temp[0] = MPU6050_GYRO_CONFIG | MPU6050_READ ;
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, temp, temp, 2, 2);
	MPU_cs_unlock(mpu);

	//On écrit
	if(mpu->hal_state == HAL_OK){
		HAL_Delay(1);
		MPU_cs_lock(mpu);
		temp[0] = MPU6050_GYRO_CONFIG ;
		temp[1] = (temp[1] & 0xE7) | (uint8_t)gyro_range << 3;
		mpu->hal_state = HAL_SPI_Transmit(mpu->hspi, temp,  2, 2);
		MPU_cs_unlock(mpu);
	}

	//ON vérifi si on a bien écris ce que l'on voulait
	if(mpu->hal_state == HAL_OK){ // on vérifi si on a bien configuré le gyro comme on voulait
		uint8_t temp2 = temp[1];
		temp[0] = MPU6050_GYRO_CONFIG | MPU6050_READ ;
		HAL_Delay(1);
		MPU_cs_lock(mpu);
		mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, temp, temp, 2, 2);
		MPU_cs_unlock(mpu);

		if(temp[1] != temp2)
		{
			mpu->state = SENSOR_ERROR ;
			return SENSOR_REQUEST_FAIL ;
		}

	}



	if(mpu->hal_state != HAL_OK)
	{
		mpu->state = SENSOR_ERROR ;
		return SENSOR_REQUEST_FAIL ;
	}

	switch(gyro_range){
		case MPU_GYRO_250s :
			mpu->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_250 ;
			break;
		case MPU_GYRO_500s :
			mpu->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_500 ;
			break;
		case MPU_GYRO_1000s :
			mpu->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_1000 ;
			break;
		case MPU_GYRO_2000s :
			mpu->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_2000 ;
			break;
	}

	return SENSOR_REQUEST_OK;
}

/*
 *
 */
sensor_request_e MPU_init_acc(mpu_t * mpu, MPU_acc_range_e acc_range, void (*acc_data_callback)){
	mpu->acc_range = acc_range ;
	mpu->acc_data_callback = acc_data_callback ;
	//Si mpu non utilisable
	if(mpu->state != SENSOR_IDDLE)
		return SENSOR_REQUEST_FAIL ;

	if(mpu->spi_speed ==  MPU_SPI_FAST)
			MPU_spi_slow(mpu);

	//Configuration de la sensi de l'acc dans le mpu
	//On lit
	uint8_t temp[2] ;
	temp[0] = MPU6050_ACCEL_CONFIG | MPU6050_READ ;
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, temp, temp, 2, 2);
	MPU_cs_unlock(mpu);

	//On écrit
	if(mpu->hal_state == HAL_OK){	//Si mpu ok on écrit en spi dans le registre de config du mpu dédié au gyro
		HAL_Delay(1);
		MPU_cs_lock(mpu);
		temp[0] = MPU6050_ACCEL_CONFIG ;
		temp[1] = (temp[1] & 0xE7) | (uint8_t)acc_range << 3;
		mpu->hal_state = HAL_SPI_Transmit(mpu->hspi, temp,  2, 2);
		MPU_cs_unlock(mpu);
	}

	//On test si on a bien écrit ce que l'on voulait
	if(mpu->hal_state == HAL_OK){ // on vérifi si on a bien configuré le gyro comme on voulait
		uint8_t temp2 = temp[1];
		temp[0] = MPU6050_ACCEL_CONFIG | MPU6050_READ ;
		HAL_Delay(1);
		MPU_cs_lock(mpu);
		mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, temp, temp, 2, 2);
		MPU_cs_unlock(mpu);

		if(temp[1] != temp2)
		{
			mpu->state = SENSOR_ERROR ;
			return SENSOR_REQUEST_FAIL ;
		}
	}



	switch(acc_range){
		case MPU_ACC_2G :
			mpu->acc_sensi = (float)1 / MPU6050_ACCE_SENS_2 ;
			break;
		case MPU_ACC_4G :
			mpu->acc_sensi = (float)1 / MPU6050_ACCE_SENS_4 ;
			break;
		case MPU_ACC_8G :
			mpu->acc_sensi = (float)1 / MPU6050_ACCE_SENS_8 ;
			break;
		case MPU_ACC_16G :
			mpu->acc_sensi = (float)1 / MPU6050_ACCE_SENS_16 ;
			break;
	}
	MPU_spi_fast(mpu);
	return SENSOR_REQUEST_OK;
}

/*
 *
 */
sensor_request_e MPU_update_gyro(mpu_t * mpu){


	if(mpu->state != SENSOR_IDDLE){
		return SENSOR_REQUEST_FAIL ;
	}

	//Update des valeurs

	uint8_t registers [] = {MPU6050_GYRO_XOUT_H | MPU6050_READ, 0, 0, 0, 0, 0};
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, registers, mpu->gyro_data, 6, 2);
	MPU_cs_unlock(mpu);

	if(mpu->hal_state == HAL_BUSY)
		return SENSOR_REQUEST_BUSY ;
	if(mpu->hal_state != HAL_OK){
		return SENSOR_REQUEST_FAIL ;
	}

	//Convertion des valeurs
	convert_gyro(mpu);
	return SENSOR_REQUEST_OK ;

}

sensor_request_e MPU_update_acc(mpu_t * mpu){

	if(mpu->state != SENSOR_IDDLE){
		return SENSOR_REQUEST_FAIL ;
	}

	//Update des valeurs
	uint8_t registers [] = {MPU6050_ACCEL_XOUT_H | MPU6050_READ, 0, 0, 0, 0, 0};
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, registers, mpu->acc_data, 6, 2);
	MPU_cs_unlock(mpu);


	if(mpu->hal_state == HAL_BUSY)
		return SENSOR_REQUEST_BUSY ;
	if(mpu->hal_state != HAL_OK){
		return SENSOR_REQUEST_FAIL ;
	}

	//Convertion des valeurs
	convert_acc(mpu);
	return SENSOR_REQUEST_OK ;
}

sensor_request_e MPU_update_all(mpu_t * mpu){
	if(mpu->state != SENSOR_IDDLE){
		return SENSOR_REQUEST_FAIL ;
	}

	//Update des valeurs
	uint8_t registers [14] = {0};
	registers[0] = MPU6050_ACCEL_XOUT_H | MPU6050_READ ;
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, registers, mpu->data, 14, 2);
	MPU_cs_unlock(mpu);


	if(mpu->hal_state == HAL_BUSY)
		return SENSOR_REQUEST_BUSY ;
	if(mpu->hal_state != HAL_OK){
		return SENSOR_REQUEST_FAIL ;
	}

	//Convertion des valeurs
	convert_acc(mpu);
	convert_gyro(mpu);
	return SENSOR_REQUEST_OK ;
}

//Update des données en DMA, ATTENTION à bien avoir un hdma associé au hspi utilisé par le mpu
sensor_request_e MPU_update_gyro_dma(mpu_t * mpu){

	uint8_t registers [] = {MPU6050_GYRO_XOUT_H | MPU6050_READ, 0, 0, 0, 0, 0};
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_TransmitReceive_DMA(mpu->hspi, registers, mpu->gyro_data, 6);

	if(mpu->hal_state == HAL_OK)
	{
		mpu->state = SENSOR_BUSY ;
		mpu->dma_state = MPU_DMA_GYRO_IN_PROGRESS ;
	}
	else if(mpu->hal_state == HAL_BUSY)
	{
		return SENSOR_REQUEST_BUSY;
	}
	else
	{
		mpu->state = SENSOR_ERROR ;
		return SENSOR_REQUEST_FAIL ;
	}


	return SENSOR_REQUEST_OK;
}
sensor_request_e MPU_update_acc_dma(mpu_t * mpu){

	uint8_t registers [6] = {0};
	registers[0] = MPU6050_ACCEL_XOUT_H | MPU6050_READ ;
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_TransmitReceive_DMA(mpu->hspi, registers, mpu->acc_data, 6);

	if(mpu->hal_state == HAL_OK)
	{
		mpu->state = SENSOR_BUSY ;
		mpu->dma_state = MPU_DMA_ACC_IN_PROGRESS ;
	}
	else if(mpu->hal_state == HAL_BUSY)
	{
		return SENSOR_REQUEST_BUSY;
	}
	else
	{
		mpu->state = SENSOR_ERROR ;
		return SENSOR_REQUEST_FAIL ;
	}

	return SENSOR_REQUEST_OK;
}

sensor_request_e MPU_update_all_dma(mpu_t * mpu){

	uint8_t registers [14] = {0};
	registers[0] = MPU6050_ACCEL_XOUT_H | MPU6050_READ ;
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_TransmitReceive_DMA(mpu->hspi, registers, mpu->data, 14);

	if(mpu->hal_state == HAL_OK)
	{
		mpu->state = SENSOR_BUSY ;
		mpu->dma_state = MPU_DMA_ACC_AND_GYRO_IN_PROGRESS;
	}
	else if(mpu->hal_state == HAL_BUSY)
	{
		return SENSOR_REQUEST_BUSY;
	}
	else
	{
		mpu->state = SENSOR_ERROR ;
		return SENSOR_REQUEST_FAIL ;
	}

	return SENSOR_REQUEST_OK;
}



static void MPU_dma_transmit_complete(mpu_t * mpu){

	//On relâche le pin CS du mpu
	MPU_cs_unlock(mpu);


	switch(mpu->dma_state){
		case MPU_DMA_GYRO_IN_PROGRESS:
			convert_gyro(mpu);
			mpu->gyro_data_callback();
			break;

		case MPU_DMA_ACC_IN_PROGRESS:
			convert_acc(mpu);
			mpu->acc_data_callback();
			break;

		case MPU_DMA_ACC_AND_GYRO_IN_PROGRESS:
			convert_gyro(mpu);
			mpu->gyro_data_callback();

			break;
		case MPU_DMA_IDDLE:
		default:
			break;

	}
	mpu->dma_state = MPU_DMA_IDDLE ;
}
static void MPU_dma_transmit_half_complete(mpu_t * mpu){

	if(mpu->dma_state == MPU_DMA_ACC_AND_GYRO_IN_PROGRESS)
	{
		//A la moitié donc les données de l'acc sont arrivées
		convert_acc(mpu);
		mpu->acc_data_callback();
	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
#if NB_MPU_USED == 1
	if(hspi == SPI_handlers[0])
		MPU_dma_transmit_complete(mpu_handlers[0]);
#else
#error Code à faire pour gérer plusieurs MPU en même temps
#endif
}

void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
#if NB_MPU_USED == 1
if(hspi == SPI_handlers[0])
	MPU_dma_transmit_half_complete(mpu_handlers[0]);
#else
#error Code à faire pour gérer plusieurs MPU en même temps
#endif

}
























