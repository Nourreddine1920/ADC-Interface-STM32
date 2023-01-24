// ADC resolution
#define ADC_RESOLUTION 12

// Potentiometer input pin
#define POT_PIN 0

// LED output pin
#define LED_PIN 13

int main(void) {
    // Enable clock for ADC1 and GPIO and CAN1
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    // Configure the potentiometer input pin as an analog input
    GPIOA->MODER |= (3 << (POT_PIN * 2));

    // Configure the LED output pin as a digital output
    GPIOD->MODER |= (1 << 24);

    // Set the ADC to use the potentiometer input pin
    ADC1->SQR3 = POT_PIN;

    // Enable the ADC
    ADC1->CR2 |= ADC_CR2_ADON;


    while(1) {
        // Start a single conversion
        ADC1->CR2 |= ADC_CR2_SWSTART;

        // Wait for the conversion to complete
        while (!(ADC1->SR & ADC_SR_EOC));

        // Read the conversion result
        uint16_t result = ADC1->DR;

        

        // Check if the voltage is greater than the threshold
        if (result > 3000) {
            // Turn on the LED
            GPIOD->BSRR = (1 << 12);
        } else {
            // Turn off the LED
            GPIOD->BSRR = (1 << (12 + 16));
        }
       }

    return 0;
}