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

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the timer 3 to generate an interrupt at a 30Hz frequency. */
void SharedTimerInitialize(void);

/** Start a non-blocking time out. The SharedTimerIsTimeOutOccurred() function will return 1 if the time is out or 0 if not.
 * @param Time How many time before the time out, in units of 100ms.
 */
void SharedTimerScheduleTimeOut(unsigned int Time);

/** Tell if the scheduled time out occurred or not.
 * @return 0 if the timer is still running,
 * @return 1 if the timer timed out.
 */
unsigned char SharedTimerIsTimeOutOccurred(void);

/** Handle the timer 3 interrupt. */
void SharedTimerInterruptHandler(void);

#endif