#include "stm32f4xx.h"
#include "stdio.h"
#include "string.h"
int ncount;
char BUFFER[100];

int n;
int i ;
int j;
char BUFFER_REC[20];




void P_ADC2d_InitADC(void)
{

  //Clock Enable (PCLK2=42MHz)
  RCC->APB2ENR |= RCC_APB2Periph_ADC2;

  //ADC2 PRESCALER=8
  ADC->CCR|= 3<<16;

  //ADC2 MODE SCAN (MULTICHANNEL)
  ADC2->CR1|= 1<<8;

  // enable mode DISCONT 3 channels à la fois aprés le start
  ADC2->CR1|= 1<<11;

  //ADC2(disable mode CONT )

  ADC2->CR2|= 0x00000000;

   //ADC2 LENGTH CHANNEL=3
   ADC2->SQR1 = 3<<20;

  //PA3 CHANNEL3 ADC2
   //ADC2->SMPR2=0x00000000;
   ADC2->SMPR1=0x00000000;
    // order of our channels 2 -> 0 -> 1
   ADC2->SQR3=0x402;


   // Enable clock for GPIOA & GPIOC
   RCC->AHB1ENR |= 1<<0;
   RCC->AHB1ENR |= 1<<2;
   
   //PA3 are analog inputs
   GPIOA->MODER |= 3 << 6;
   //PC4 PC5 are analog inputs
   GPIOC->MODER |= 3<<8;
   GPIOC->MODER |= 3<<10;

   //eocue=1
   /* Set the ADON bit to wake up the ADC from power down mode */
   ADC2->CR2 |= 1<<0; // ADC ON 
}
//________________________________________________________________________________________________________

void config_usart()
{
  // Enable clock for GPIOB
  RCC->AHB1ENR |= 0x00000002;
  // Enable clock for USART3
  RCC->APB1ENR|=0x00040000;
  //enable USART3_TX to PB10 and USART3_RX to PB11
  GPIOB->AFR[1]=0x00007700;
  // configuring the USART3 ALTERNATE function  to PB10 and PB11
  GPIOB->MODER|=0x2AA00000;
  //  BaudRate:9600
  USART3->BRR=0x1120;
  //834
  //USART3->BRR     = 0x0890;      // 19200 baud

  // USART3 enable
  USART3->CR1|=0x0000203C;
  NVIC_EnableIRQ(USART3_IRQn);
}

//_________TRANSMISSION D'UN CARACTERE_______________________________________________________

void SendChar(char Tx)
{
	while((USART3->SR&0x80)==0);  // On attend à ce que TXBUFF soit vide (libere)ou(FLAG TXNE=1) ou Le caractere est envoyé
	USART3->DR=Tx;
}

//____________TRANSMISSION STRING________________________________________________

void Sendstring(char *Adr)
{
  while(*Adr)
  {
    SendChar(*Adr);
    Adr++;
  }
}

//_____________________________________________________________________________________________________

void Systemclock() /// CLK_TIMER=84MHz
{

  RCC->CFGR |= 0x00009400;        // AHB_presc=1  APB1_presc=4
  RCC->CR |= 0x00010000;          // HSE_ON
  while (!(RCC->CR & 0x00020000));// wait until HSE READY

  RCC->PLLCFGR = 0x07402A04;      // PLL  M=4, N=168, P=2  Q=7    168 Mhz
  RCC->CR |= 0x01000000;          // PLL_ON
  while(!(RCC->CR & 0x02000000)); // wait Pll_ready

  FLASH->ACR = 0x00000605;        // Flash ROM
  RCC->CFGR |= 0x00000002;        // Sistem Clk u PLL uzerinden besleyelim
  while ((RCC->CFGR & 0x0000000F) != 0x0000000A); // Besleninceye kadar bekle

}

//______________________________________________________________________

void config_TIM3()
{
	 // Enable TIM3 clock (RCC_APB1ENR[1]=1)
	 RCC->APB1ENR |= 1<<1;//le bit [1] du registre APB1ENR
	 // Set prescaler to 7999
	 TIM3->PSC = 7999; // prescaler=8000 sachant syst clk=HSE  => counter clk= 1000Hz
	 // Set ARR to 999
	 TIM3->ARR = 999; // le compteur compte de 0 à 999
	 //ENABLE counter CCR=<ARR , de CH1,CH2,CH3
	 TIM3->CCER|=0x0111; //!!!!!!!!!!!!!
	 // CH1,CH2 output mode 1
	 TIM3->CCMR1|=0x6060;
	 // CH3 output mode 1
	 TIM3->CCMR2|=0x0060;

	 //ACTIVATION CLOCK GPIOB <-------------------------------------UP DATE
	 RCC->AHB1ENR |= 0x00000003;

	 //PA6 et PA7 en mode alternate function
	 GPIOA->MODER|=0x0000A000;
	 // lier TIM3 CH1(AF2) à PA6 et CH2(AF2) à PA7 connectée
	 GPIOA->AFR[0]|=0x33000000;

	 //CONFIG PIN0 GPIOB EN MODE ALTERNATE FUNCTION <------------------------UPDATE
	 GPIOB->MODER|=0x00000002;
	 // lier TIM3 CH3 (AF2) à PB0
	 GPIOB->AFR[0]|=0x00000003;


	 // ENABLE counter CNT=ARR bit tim2[0]= CEN=1
	TIM3->CR1= 0x0001;

}
//______________________________________________________________________________________________________

void config_EXTI()
{

}

//_________________________________________________________________________________________

Void config_PWM()
{
		RCC->APB1ENR = 0x2; //clock enable for TIM3
	   RCC->AHB1ENR = 0x1; //clock enable GPIOA
	   GPIOA->MODER = 0xA000; //Alternative function mode PA6 and PA7
	   TIM3->CCMR1 = 0x7070; //PWM mode 1 on TIM3 Channel 1 & TIM3 Channel 2
	   TIM3->PSC = 1000;
	   TIM3->ARR = 200;
	   TIM3->CCR1 = 0;
	   TIM3->CCR2 = 0;
	   TIM3->CCR3 = 0;
	   GPIOA->AFR[0] = 0x22000000; //set GPIOA to AF2
	   TIM3->CCER=0x11; //Enable Capture/Compare 1 output  &&   Enable Capture/Compare 1 output
	   TIM3->EGR=0x06;  // Capture/compare 1 generation && Capture/compare 2 generation
	   TIM3->CR1= 0x1; //enable timer 3
}

//____________________________________________________________________________________________


int main(void)
{
	//System clock : HSExPLL=8x21=168MHz
	  Systemclock();

	  config_usart();
	  config_TIM3();
	  config_PWM();
	  P_ADC2d_InitADC();


    while(1)
    {
        	TIM3->CCR1 ++;
        	TIM3->CCR2 ++;
        	TIM3->CCR3 ++;
        	if (TIM3->CCR1==TIM3->ARR && TIM3->CCR2==TIM3->ARR && TIM3->CCR==TIM3->ARR)
        	{
        	        	TIM3->CCR1=0;
        	        	TIM3->CCR2=0;
        	        	TIM3->CCR3=0;
        	        }
        	        for(f=0; f<100000; f++);

        }
}
//_________PARTIE HANDLER___________________________________________________________



void EXTI0_IRQHandler()
 {
	 j++;
	 if(j==1)
	 {
		 EN_TIMER=1;
	 }
	 if(j==2)
	 {
		 j=0;
		 EN_TIMER=0;
	 }
 }


















