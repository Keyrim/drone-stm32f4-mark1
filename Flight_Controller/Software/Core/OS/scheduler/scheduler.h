/*
 * scheduler.h
 *
 *  Created on: 12 ao�t 2020
 *      Author: Theo
 */

#ifndef SCHEDULER_SCHEDULER_H_
#define SCHEDULER_SCHEDULER_H_

#include "../tasks/task.h"
#include "../macro_types.h"
#include "../debug/debug.h"
#include "../system_d.h"
#include "string.h"



void SCHEDULER_init(system_t * sys_);
void SCHEDULER_run(void);

void SCHEDULER_task_set_mode(task_ids_t id, task_mode_e mode);
void SCHEDULER_enable_task(task_ids_t id, bool_e enable);
void SCHEDULER_reschedule_task(task_ids_t id, uint32_t new_period_us);

void SCHEDULER_task(void);

float SCHEDULER_get_cpu_load(void);

#endif /* SCHEDULER_SCHEDULER_H_ */
