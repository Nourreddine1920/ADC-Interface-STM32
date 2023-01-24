#include "stm32f4xx.h"

#include "stdio.h"

  int i=1;
  char BUFFER[100];
  int ADC_VALUE[3];
  int TEMPERATURE=0;

  void GPIO_CONFIG(void);
  void TIMER2_CONFIG(void);
  void ADC_CONFIG(void);
  void SysCLK_CONFIG(void);
  void USART_CONFIG(void);
  void INTERRUPT_CONFIG(void);
  void PWM_GEN(void);

  void SysCLK_CONFIG(void)
  {
  	//Enable HSI
  	RCC->CR |= (0x1<<0);
  	//HSI oscillator selected as system clock
  	RCC->CFGR &= (0xC<<0);
  	while (!(RCC->CR & 0x00000002));
  	//AHB1PSC
  	RCC->CFGR |=(0x0<<4);
  	//APB1PSC
  	RCC->CFGR |=(0x0<<10);
  	//APB2PSC
  	RCC->CFGR |=(0x0<<13);
  }
  void GPIO_CONFIG(void)
  {
  	//Enable clock GPIO:A
  	RCC->AHB1ENR |=(0x1<<0);
  	GPIOA->MODER |=(0x3<<2);//INPUT1 ADC PA1
    GPIOA->MODER |=(0x3<<4);//INPUT2 ADC PA2
    GPIOA->MODER |=(0x3<<6);//INPUT3 ADC PA3

  	//BEGIN PWM
  	GPIOA->MODER |=(0x2A<<2);
  	GPIOA->AFR[0] &= (0x000<<4);
  	GPIOA->AFR[0] |= (0x222<<4);
  	//END PWM

  	/*Enable clock GPIO:D*/
  	RCC->AHB1ENR |=(1<<3);
  	GPIOD->MODER = 0x00000000;
  	GPIOD->MODER |=(0x1<<24);
  	GPIOD->MODER |=(0x1<<26);
  	GPIOD->MODER |=(0x1<<28);
  	GPIOD->MODER |=(0x1<<30);
  }
  void TIMER2_CONFIG(void)
  {
  	//Enable clock TIM2
  	RCC->APB1ENR |=(1<<0);
  	//TIM2->CR1 &=(0<<4);
  	//Timer Pre-scaler
  	TIM2->PSC =0xF519;
  	//Interrupt Enable
  	//TIM2->DIER |=(0x0001<<0);
  	//Timer ARR
  	TIM2->ARR = 255;
  	//NVIC interrupt request
  	//NVIC_EnableIRQ(TIM2_IRQn);
  	//NVIC_SetPriority(TIM2_IRQn, 0x50);
  }
/*  void ADC1_CONFIG(void)
  {
  	//Enable clock ADC1
  	RCC->APB2ENR |=(1<<8);
	//pre-scaler + Delay
	ADC->CCR =0x00000000;
  	//Mode discontinue
  	ADC1->CR2 &= (0<<1);
	//The EOC bit is set at the end of each regular conversion
	ADC1->CR2 |= (1<<10);
  	//Resolution de conversion: Res = 8
  	ADC1->CR1 &= (0<<24);
  	ADC1->CR1 |= (1<<25);
  	//Mode Scan
  	ADC1->CR1 |= (1<<8);
  	//Period d'chentillonage: 3 cycles
  	ADC1->SMPR1 &=(0x000);
  	//nombre des echantillons: nbr = 3
  	ADC1->SQR1 &=(0x0<<20);
  	ADC1->SQR1 |=(0x2<<20);
  	//Sequence des echentillons:ch1=>ch2=>ch3
  	ADC1->SQR3 &=(0x0000);
  	ADC1->SQR3 |=(0x1<<0);
  	ADC1->SQR3 |=(0x2<<5);
  	ADC1->SQR3 |=(0x3<<10);
  	//EOCIE=1
  	ADC1->CR1 |=(1<<5);
  	//NVIC interrupt request
  	NVIC_EnableIRQ(ADC_IRQn);
  }*/
  void ADC1_CONFIG()
  {
		/*Enable clock ADC1*/
		RCC->APB2ENR |=(1<<8);
		/*pre-scaler + Delay*/
		ADC->CCR =0x00000000;
  		/*continuous mode channel count nbr = 1*/
  		//ADC1->CR1 &= (0x1<<13);
  		ADC1->CR2 |= (0x1<<1);
		//The EOC bit is set at the end of each regular conversion
		ADC1->CR2 |= (1<<10);
	  	//Resolution de conversion: Res = 8
	  	ADC1->CR1 &= (0<<24);
	  	ADC1->CR1 |= (1<<25);
		/*Mode Scan*/
		ADC1->CR1 |= (1<<8);
		/*Period d'chentillonage=  3 cycles*/
		ADC1->SMPR2 =0x00000000;
		/*nombre des echantillons: nbr = 3*/
		ADC1->SQR1 |=(0x2<<20);
		/*EOCIE=1*/
		ADC1->CR1 |=(1<<5);
		/*NVIC interrupt request*/
		NVIC_EnableIRQ(ADC_IRQn);
		//NVIC_SetPriority(ADC_IRQn, 0x00);
  }
  void Usart3Init(){
   RCC->AHB1ENR |= (1<<1); // Enable clock for GPIOB
   RCC->APB1ENR|=0x00040000; // Enable clock for USART3
   GPIOB->AFR[1]=(0x77<<8); // //enable USART3_TX to PB10 and USART3_RX to PB11
   GPIOB->MODER|=0x2AA00000; // configuring the USART3 ALTERNATE function to PB10 and PB11
   USART3->BRR = 0x008b; // 115200 baud
   USART3->CR1|=(0x202C<<0); // USART3 enable
  }
  void SendMassege(char STRING[100]){
   	 int c=0;
   	 while(STRING[c]!='\0'){
   		 while((USART3->SR&0x80)==0);
   		 USART3->DR=STRING[c];
   		 c++;
   	 };
   }
  void ADC_IRQHandler(void)
   {
 	 if(ADC1->SR&(1<<1))
 	 {
 		 if(i==1){
 			//while(ADC1->SR&(1<<1)){
 			 ADC_VALUE[0]=ADC1->DR;
 			//};
 			ADC1->SQR3=(2<<0);
  			ADC1->SR&=(0<<1);
 			//ADC1->CR2 |=(1<<30);
 		 }
 		 if(i==2){
 			//while(ADC1->SR&(1<<1)){
 			 ADC_VALUE[1]=ADC1->DR;
 			//};
 			ADC1->SQR3=(3<<0);
  			ADC1->SR&=(0<<1);
 			//ADC1->CR2 |=(1<<30);
 		 }
 		 if(i==3){
 			i=1;
 			ADC_VALUE[2]=ADC1->DR;
 			ADC1->SQR3=(1<<0);
 			TIM2->SR &=(0<<1);
 			TIM2->CR1|=(1<<0);
 			ADC1->SR&=(0<<1);
 		  	/*Disable ADC ADON=0*/
 		  	ADC1->CR2 &= (0<<0);
 		  	sprintf(BUFFER,"SPEED1 =%d\nSPEED2 =%d\nSPEED3 =%d",ADC_VALUE[0],ADC_VALUE[1],ADC_VALUE[2]);
 			PWM_GEN();
 		 }else{i++;ADC1->CR2 |=(1<<30);}
 	 }
   }
  void PWM_GEN(void)
  {
	  while(!(TIM2->SR & (1<<1))){
		  	if(TIM2->CNT<=ADC_VALUE[0]){GPIOD->ODR = (1<<12);}
		  	if(TIM2->CNT<=ADC_VALUE[1]){GPIOD->ODR = (1<<13);}
		  	if(TIM2->CNT<=ADC_VALUE[2]){GPIOD->ODR = (1<<14);}
		  	GPIOD->ODR = (0<<12);
		  	GPIOD->ODR = (0<<13);
		  	GPIOD->ODR = (0<<14);
	  }
	  	SendMassege(BUFFER);
	  	/*Enable ADC ADON=1*/
	  	ADC1->CR2 |= (1<<0);
	  	ADC1->CR2|=(1<<30);
  }
  void Delay(int Time){
    	int t = Time*1600;
    	TIM3->SR &=(0<<0);
     	//Enable clock TIM2
     	RCC->APB1ENR |=(1<<1);
     	//Timer Pre-scaler
     	TIM3->PSC =0x2710;
     	//Timer ARR
     	TIM3->ARR = t;
     	TIM3->CR1 |=(1<<0);
    	while(!(TIM3->SR &&(1<<0)));
    }

  int main()
  {
	  SysCLK_CONFIG();
	  GPIO_CONFIG();
	  TIMER2_CONFIG();
	  ADC1_CONFIG();
	  Usart3Init();
	  /*Enable ADC ADON=1*/
	  ADC1->CR2 |= (1<<0);
	  ADC1->CR2|=(1<<30);
	  while(1);
  }

