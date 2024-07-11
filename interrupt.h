#include "env.h"


#ifndef interrupt

#define interrupt
volatile bool INTERRUPT_FLAG = false; // 인터럽트 발생 여부를 저장할 변수

void IRAM_ATTR handleInterrupt()
{
    INTERRUPT_FLAG = true;                      // 인터럽트가 발생했음을 기록
    detachInterrupt(digitalPinToInterrupt(ENV::BTN)); // 인터럽트 비활성화
};

#endif
