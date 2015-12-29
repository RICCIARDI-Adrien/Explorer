/** @file Shared_Timer.h
 * Use a timer interrupt to schedule multiple tasks in order to avoid generating too much interrupts due to multiple timers.
 * @author Adrien RICCIARDI
 */
#ifndef H_SHARED_TIMER_H
#define H_SHARED_TIMER_H

//--------------------------------------------------------------------------------------------------
// Constants
//--------------------------------------------------------------------------------------------------
/** Enable the timer 3 interrupt. */
#define SHARED_TIMER_ENABLE_INTERRUPT() pie2.TMR3IE = 1
/** Disable the timer 3 interrupt. */
#define SHARED_TIMER_DISABLE_INTERRUPT() pie2.TMR3IE = 0

/** How many software timers are available. */
#define SHARED_TIMER_TIMERS_COUNT 5

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the timer 3 to generate an interrupt at a 30Hz frequency. */
void SharedTimerInitialize(void);

/** Start a non-blocking software timer. The timer will count from Time to 0. The SharedTimerIsTimerStopped() function will return 1 if the timer reached 0 or 0 if not.
 * @param Index Which timer to start. There are up to SHARED_TIMER_TIMERS_COUNT timers. The function does nothing if an invalid index is provided.
 * @param Time How many time before the timer times out, in units of 100ms.
 */
void SharedTimerStartTimer(unsigned char Index, unsigned int Time);

/** Tell if the specified timer stopped or not.
 * @param Index The timer to test. There are up to SHARED_TIMER_TIMERS_COUNT timers.
 * @return 0 if the timer is still running or an invalid index is provided,
 * @return 1 if the timer timed out.
 */
unsigned char SharedTimerIsTimerStopped(unsigned char Index);

/** Handle the timer 3 interrupt. */
void SharedTimerInterruptHandler(void);

#endif