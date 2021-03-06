#include <advinputmanager_arm.h>

#include "providers_settings.h"

#define DELAY_TIMER TIM4
#define DELAY_TIMER_APB APB1ENR
#define DELAY_TIMER_APBEN RCC_APB1ENR_TIM4EN

// #define ADVIM_DEBUG 1
// #define ADVIM_PRINTF myprintf
#define ADVIM_INPUTSCOUNT INPUTS_COUNT 
// #define ADVIM_PATTERNENABLED
// #define ADVIM_PATTERNMAX 20


#define OW_USART       USART3
#define OW_USART_APB   APB1ENR
#define OW_USART_APBEN RCC_APB1ENR_USART3EN

// IP settings
#define HW_ADDR "\x00\xC4\xDE\x6D\xD5\xEF"
// #define IP_ADDR 192,168,1,50
// #define IP_MASK 255,255,255,0
// #define IP_GW   192,168,1,1
// #define IP_SRV  192,168,100,1
#define IP_SRV  192,168,2,2

#define LOCAL_PORT 9999
#define SRV_PORT 9999

#define ADVIM_INPUT0_ADDR IN1
#define ADVIM_INPUT0_FLAGS 0
#define ADVIM_INPUT0_DEBOUNCETIME 20
#define ADVIM_INPUT0_PATTERNTIMEOUT 0
#define ADVIM_INPUT1_ADDR IN2
#define ADVIM_INPUT1_FLAGS 0
#define ADVIM_INPUT1_DEBOUNCETIME 20
#define ADVIM_INPUT1_PATTERNTIMEOUT 0
#define ADVIM_INPUT2_ADDR IN3
#define ADVIM_INPUT2_FLAGS 0
#define ADVIM_INPUT2_DEBOUNCETIME 20
#define ADVIM_INPUT2_PATTERNTIMEOUT 0
#define ADVIM_INPUT3_ADDR IN4
#define ADVIM_INPUT3_FLAGS 0
#define ADVIM_INPUT3_DEBOUNCETIME 20
#define ADVIM_INPUT3_PATTERNTIMEOUT 0
#define ADVIM_INPUT4_ADDR IN5
#define ADVIM_INPUT4_FLAGS 0
#define ADVIM_INPUT4_DEBOUNCETIME 20
#define ADVIM_INPUT4_PATTERNTIMEOUT 0
#define ADVIM_INPUT5_ADDR IN6
#define ADVIM_INPUT5_FLAGS 0
#define ADVIM_INPUT5_DEBOUNCETIME 20
#define ADVIM_INPUT5_PATTERNTIMEOUT 0
#define ADVIM_INPUT6_ADDR IN7
#define ADVIM_INPUT6_FLAGS 0
#define ADVIM_INPUT6_DEBOUNCETIME 20
#define ADVIM_INPUT6_PATTERNTIMEOUT 0
#define ADVIM_INPUT7_ADDR IN8
#define ADVIM_INPUT7_FLAGS 0
#define ADVIM_INPUT7_DEBOUNCETIME 20
#define ADVIM_INPUT7_PATTERNTIMEOUT 0
