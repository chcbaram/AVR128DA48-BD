#include "sw_timer.h"


#ifdef _USE_HW_SW_TIMER

typedef struct
{

  bool     timer_en;             // 타이머 인에이블 신호
  SwTimerMode_t timer_mode;      // 타이머 모드
  uint32_t timer_ctn;            // 현제의 타이머 값
  uint32_t timer_init;           // 타이머 초기화될때의 카운트 값
  void (*func)(void *);          // 만료될때 실행될 함수
  void *func_arg;                // 함수로 전달할 인수들
} sw_timer_t;



static volatile uint32_t sw_timer_counter      = 0;
static volatile uint8_t  sw_timer_handle_index = 0;
static sw_timer_t  sw_timer_tbl[SW_TIMER_MAX_CH];           // 타이머 배열 선언




bool swTimerInit(void)
{
  uint8_t i;
  static uint8_t excute = 0;


  if (excute == 1)
  {
    return false;  // 이미 한번 실행했다면 정지.
  }


  // 구조체 초기화
  for(i=0; i<SW_TIMER_MAX_CH; i++)
  {
    sw_timer_tbl[i].timer_en   = false;
    sw_timer_tbl[i].timer_ctn  = 0;
    sw_timer_tbl[i].timer_init = 0;
    sw_timer_tbl[i].func       = NULL;
  }

  excute = 1;

  return true;
}

void swTimerISR(void)
{
  uint8_t i;


  sw_timer_counter++;


  for (i=0; i<SW_TIMER_MAX_CH && i<sw_timer_handle_index; i++) // 타이머 갯수만큼
  {
    if ( sw_timer_tbl[i].timer_en == true)                      // 타이머가 활성화 됬니?
    {
      sw_timer_tbl[i].timer_ctn--;                              // 타이머값 감소

      if (sw_timer_tbl[i].timer_ctn == 0)                       // 타이머 오버플로어
      {
        if(sw_timer_tbl[i].timer_mode == ONE_TIME)              // 한번만 실행하는거면
        {
          sw_timer_tbl[i].timer_en = false;                     // 타이머 OFF 한다.
        }

        sw_timer_tbl[i].timer_ctn = sw_timer_tbl[i].timer_init; // 타이머 초기화

        (*sw_timer_tbl[i].func)(sw_timer_tbl[i].func_arg);     // 함수 실행
      }
    }
  }
}

void swTimerSet(uint8_t ch, uint32_t timer_ms, SwTimerMode_t timer_mode, void (*func)(void *), void *arg)
{
  sw_timer_tbl[ch].timer_mode = timer_mode;    // 모드설정
  sw_timer_tbl[ch].func       = func;          // 실행할 함수
  sw_timer_tbl[ch].func_arg   = arg;           // 매개변수
  sw_timer_tbl[ch].timer_ctn  = timer_ms;
  sw_timer_tbl[ch].timer_init = timer_ms;
}

void swTimerStart(uint8_t ch)
{
  if(ch < SW_TIMER_MAX_CH)
  {
    sw_timer_tbl[ch].timer_ctn = sw_timer_tbl[ch].timer_init;
    sw_timer_tbl[ch].timer_en  = true;
  }
}

void swTimerStop (uint8_t ch)
{
  if(ch < SW_TIMER_MAX_CH)
  {
    sw_timer_tbl[ch].timer_en = false;
  }
}

void swTimerReset(uint8_t ch)
{
  sw_timer_tbl[ch].timer_en   = false;
  sw_timer_tbl[ch].timer_ctn  = sw_timer_tbl[ch].timer_init;
}

uint8_t swtimerGetHandle(void)
{
  uint8_t ret = sw_timer_handle_index;

  if (ret < SW_TIMER_MAX_CH)
  {
    sw_timer_handle_index++;
  }

  return ret;
}

uint32_t swtimerGetCounter(void)
{
  return sw_timer_counter;
}

#endif