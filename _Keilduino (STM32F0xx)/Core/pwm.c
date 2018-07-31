#include "pwm.h"

//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIMx_Init(TIM_TypeDef* TIMx,uint16_t arr, uint16_t psc,uint8_t CHx)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

			 if(TIMx == TIM1)RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	else if(TIMx == TIM2)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if(TIMx == TIM3)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
#if defined(TIM4)
	else if(TIMx == TIM4)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
#endif
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	switch(CHx)
	{
		case 1:
			TIM_OC1Init(TIMx, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;
		case 2:
			TIM_OC2Init(TIMx, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;
		case 3:
			TIM_OC3Init(TIMx, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;
		case 4:
			TIM_OC4Init(TIMx, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;
	}
	
	TIM_Cmd(TIMx, ENABLE);
	if(TIMx==TIM1)TIM_CtrlPWMOutputs(TIMx, ENABLE);
}

uint8_t PWM_Init(uint8_t Pin, uint16_t PWM_DutyCycle, uint16_t PWM_Frequency)
{
	uint32_t arr,psc;
	
	if(!IS_PWM_PIN(Pin))return 0;
	
	pinMode(Pin,OUTPUT_AF);
	GPIO_PinAFConfig(PIN_MAP[Pin].GPIOx, Get_Pinx(Pin), Get_TIMx_GPIO_AF_x(Pin));
	
	arr = PWM_DutyCycle;
	psc = F_CPU / PWM_DutyCycle / PWM_Frequency;
	
	TIM_Cmd(PIN_MAP[Pin].TIMx, DISABLE);
	TIMx_Init(PIN_MAP[Pin].TIMx,arr-1,psc-1,PIN_MAP[Pin].TimerChannel);
	return PIN_MAP[Pin].TimerChannel;
}

uint16_t pwmWrite(uint8_t Pin,uint16_t val)
{
	switch(PIN_MAP[Pin].TimerChannel)
	{
		case 1: PIN_MAP[Pin].TIMx->CCR1 = val;break;
		case 2: PIN_MAP[Pin].TIMx->CCR2 = val;break;
		case 3: PIN_MAP[Pin].TIMx->CCR3 = val;break;
		case 4: PIN_MAP[Pin].TIMx->CCR4 = val;break;
	}
	return val;
}

uint8_t Get_TIMx_GPIO_AF_x(uint8_t Pin)
{
	uint8_t GPIO_AF_x = 0;
	TIM_TypeDef* TIMx = PIN_MAP[Pin].TIMx;
	
	if(TIMx == TIM14 || TIMx == TIM15 || TIMx == TIM17)
	{
		GPIO_AF_x = GPIO_AF_0;
	}
	else if(TIMx == TIM3 || TIMx == TIM15)
	{
		GPIO_AF_x = GPIO_AF_1;
	}
	else if(TIMx == TIM1 || TIMx == TIM2 || TIMx == TIM16 || TIMx == TIM17)
	{
		GPIO_AF_x = GPIO_AF_2;
	}
	else if(TIMx == TIM15)
	{
		GPIO_AF_x = GPIO_AF_3;
	}
	else if(TIMx == TIM14)
	{
		GPIO_AF_x = GPIO_AF_4;
	}
	else if(TIMx == TIM16 || TIMx == TIM17)
	{
		GPIO_AF_x = GPIO_AF_5;
	}
	
	return GPIO_AF_x;
}

//*************************//

uint16_t timer_get_compare(TIM_TypeDef* TIMx,uint8_t TimerChannel)
{
	uint16_t compare=0;
	switch(TimerChannel)
	{
		case 1: compare = TIMx->CCR1;break;
		case 2: compare = TIMx->CCR2;break;
		case 3: compare = TIMx->CCR3;break;
		case 4: compare = TIMx->CCR4;break;
	}
	return compare;
}

void timer_set_prescaler(TIM_TypeDef* TIMx,uint16_t psc)
{
   TIMx->PSC = psc;
}

void timer_set_reload(TIM_TypeDef* TIMx,uint16_t arr)
{
  TIMx->ARR = arr;
}

void timer_generate_update(TIM_TypeDef* TIMx)
{
	TIMx->EGR = TIM_PSCReloadMode_Immediate; 
}
