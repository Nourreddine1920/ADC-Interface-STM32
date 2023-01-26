## ADC INTERFACE USING STM32 WITH REGISTERS 

ADC interface with DMA with STM32 using registers

### STEPS TO FOLLOW 

1. Enable ADC and GPIO clock
2. Set the prescalar in the Common Control Register (CCR)
3. Set the Scan Mode and Resolution in the Control Register 1 (CR1)
4. Set the Continuous Conversion, EOC, and Data Alignment in Control Reg 2 (CR2)
5. Set the Sampling Time for the channels in ADC_SMPRx
6. Set the Regular channel sequence length in ADC_SQR1
7. Set the Respective GPIO PINs in the Analog Mode

