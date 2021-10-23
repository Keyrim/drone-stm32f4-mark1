/*
 * scheduler.h
 *
 *  Created on: Oct 23, 2021
 *      Author: Theo
 */

#ifndef SCHEDULER_SCHEDULER_H_
#define SCHEDULER_SCHEDULER_H_

#include "../tasks/task.h"
#include "../macro_types.h"
#include "../system_d.h"
#include "string.h"



void SCHEDULER_init(void);
void SCHEDULER_run(void);
void SCHEDULER_task_set_mode(task_ids_e id, task_mode_e mode);
void SCHEDULER_enable_task(task_ids_e id, bool_e enable);
void SCHEDULER_reschedule_task(task_ids_e id, uint32_t new_period_us);
void SCHEDULER_task(void);

float SCHEDULER_get_cpu_load(void);

#endif /* SCHEDULER_SCHEDULER_H_ */
