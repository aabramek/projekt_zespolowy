#ifndef MAIN_H
#define MAIN_H

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
	
#define PERIOD_TIMER_ID (void*)0xDEADBEEF

extern TimerHandle_t _period_timer;

void PeriodTimerCallback(TimerHandle_t timer)
{
	
}

#endif