/*
 * events.c
 *
 *  Created on: Oct 23, 2021
 *      Author: Theo
 */

#include "mask_def.h"
#include "flags.h"
#include "events.h"
#include "../scheduler/scheduler.h"


#define DEFINE_EVENT(event_function_param, nb_mask_param, enable_param)\
{  																\
		.function = event_function_param,						\
		.nb_mask = nb_mask_param, 								\
		.state = enable_param									\
}

/* Flags */
volatile static Mask_t flags ;
volatile static bool_e new_flag = FALSE ;

/* To avoid forgetting to call the initialization function */
static bool_e initialized = FALSE ;


//	----------------------	Events Declaration ---------------------------------------------------------

static Event_t events_main[EVENT_COUNT] =
{
		/* Event array */

};


/*
 * @brief Initialize the masks
 */
void EVENT_init()
{
	/* Masks configuration */
	MASK_DEF_init(events_main);
	initialized = TRUE ;
}

/*
 * @brief check if any event has been triggered
 * @param test_all False to test only low level events
 */
void EVENT_process(bool_e test_all){
	if(new_flag)
	{
		new_flag = FALSE;
		/* Number of events to test */
		uint8_t max = EVENT_LOW_LVL_COUNT ;
		if(test_all)
		{
			max = EVENT_COUNT ;
		}

		/* For each events */
		for(uint32_t e = 0; e < max; e ++)
		{

			uint32_t m = 0 ;
			bool_e function_did_run_once = FALSE ;

			/* For each masks */
			while(m < events_main[e].nb_mask && !function_did_run_once && events_main[e].state == event_state_eENABLED)
			{
				if(Mask_test_and(events_main[e].mask_and[m], flags))			/* Mask "and" test */
				{
					if(Mask_test_or(events_main[e].mask_or[m], flags))			/* Mask "or" test */
					{
						if(!Mask_test_or(events_main[e].mask_not[m], flags))	/* Mask "not" test */
						{
							events_main[e].function(m);
							function_did_run_once = TRUE ;
						}
					}
				}
				m++ ;
			}
		}
	}
}

/*
 * @brief Get the state of a flag
 * @param flag Well, THE flag ..
 * @return The state of the flag
 */
bool_e EVENT_Check_flag(flag_e flag)
{
	return Mask_test_and(MASK_create_single(flag), flags);
}

/*
 * @brief Set a flag
 * @param flag The flag to set
 */
void EVENT_Set_flag(flag_e flag)
{
	__disable_irq();					/* Disable ITs to avoid reentrance */
	new_flag = TRUE ;
	MASK_set_flag(&flags, flag);
	__enable_irq();
}
/*
 * @brief Clear a flag
 * @param flag the flag to clear
 */
void EVENT_Clear_flag(flag_e flag)
{
	__disable_irq();					/* Disable ITs to avoid reentrance */
	new_flag = TRUE ;
	MASK_clean_flag(&flags, flag);
	__enable_irq();
}








