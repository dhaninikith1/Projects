/*********************************************************************
 *
 *  	PIC32MX PWM Output Demo
 *
 *********************************************************************
 * FileName:    	pwm_servo_output_mx7.c
 * Dependencies:	plib.h
 *
 * Processor:   	PIC32MX795F512L
 *
 *
 *
 *********************************************************************
 * The purpose of this example code is to demonstrate using the PIC32MX
 * to drive servo motors using timer 1 to generate a syncorinizing interrupt
 * and providing varying pulse width outputs to the PMOD connector that will
 * be connected to the servo interface unit.
 * The servo interface unit is driven by digital IO ports
 * rather than the output compare modules that can also generate PWM signals.
 *
 * Platform: ChipKIT MX7
 *
 * Features demonstrated:
 *	- timer driven PWM signals
 *	- Use of the c peripheral library functions
 *
 * Description:
 *  	This program allows a user to vary the pulse width of signals
 *  	that connect to the servo motor connector bank on the MX7 board.
 *  	The pulse width is controlled by a potentiometer connected to AN4,
 *  	converted by the ADC to binary value, which is then
 *  	used to vary the time during which the output pulse is high.
 *  	The period of the PWM signal is controlled by timer 1.
 *
 ********************************************************************/
/*  Modified: 11/25/2018 by E.J. Nava, University of New Mexico
 *  for classroom demonstration purposes.
 *
 *******************************************************************/

#include <plib.h>
#include <stdio.h>

// Configure MX7 board for debugging
#pragma config ICESEL = ICS_PGx1

#pragma config FNOSC = PRIPLL    	// Configure the system clock
#pragma config POSCMOD = EC
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config FPLLODIV = DIV_1
#pragma config FPBDIV = DIV_8
#pragma config FSOSCEN = OFF        	// Secondary oscillator enable
#define SYS_FREQ (80000000L)

#define PB_DIV    	8        	// 10 MHz peripheral clock
                              
#define	cntMsDelay	10000    	//timer 1 delay for 1ms

#define PRESCALE	256            	// 312500 Hz clock to timer
#define SECONDS_PER_UPDATE	1    	// time interval for terminal update

#define T4_TICK    	((SYS_FREQ/PB_DIV/PRESCALE)*SECONDS_PER_UPDATE)

#define SPIBRG 31           	// value of SPI BRG for 156.25kHz with
                                // peripheral clock at 10Mhz
#define I2CBRG 0x009        	// baud rate for 10Mhz clock to I2C                                                   
#define POT 4                    	// analog input on AN4
#define AINPUTS 0xffef    	// set all bits 1 except for AN5 line
#define ADC_COUNT 1023    	// number of values represented by ADC
#define VMAX 3.2        	// Vref +
#define VMIN 0.0        	// Vref -

// global vaiables
int servoflag = 0;  	// used in main program to indicate PWM generation
unsigned char i2c_data[4];    	// buffer for TMP3 data
unsigned char SlaveAddress;    	// I2C slave device TMP3 address
unsigned char *in_char_p; 
int time = 0;
int update = 0;
int oneSecFlag = 0;     
double temperature,fuel;
   
// function prototypes
void initADC( int amask);
int readADC( int c);
void DelayMs(int cms);
void DelayInit();
void DeviceInit();
void SPIInit();
void UARTInit();
void I2CInit();
double readI2C();
void TimersInit();
void WriteSpi(double temperature, double fuel, int overheat);
void ServoDelay(short int counts);
void OC2Init();
void MoveServo(int adc);

void __ISR(_TIMER_4_VECTOR, ipl2) Timer4Handler(void)
{
	mT4ClearIntFlag();
	PORTToggleBits(IOPORT_G, BIT_12);
	oneSecFlag = 1;
	time = time + 1;
	if (time % 10 == 0)
	{
    	update = 1;
	}
}

int main(void)
{
	extern int servoflag;   	// indicator to start PWM delays   
	unsigned int tcfg2;     	// timer configuration word
	unsigned int adc_value; 	// adc input value 0..0x400
	short int counts1;            	// timer value for servo pulse 1 width
	int overheat;
   
	fuel = 5000;
	//initialize timer 1 for interrupt operation to drive servos

	//initialize output port G for servo control signal
	// **** TEST with PORT B *****
	DeviceInit();
	TimersInit();
	SPIInit();
	UARTInit();
	I2CInit();
	//initialize timer 4 for delay
	OC2Init();
   // initialize the ADC  
	initADC(AINPUTS);        	// initialize the ADC
   

   
	// sample and OC1 duty cycle control loop
   
	while (1) {
       	// read data from ADC
    	temperature = readI2C();
    	if (temperature < 80){

         	overheat = 0;
    	}
    	else overheat = 1;
    	adc_value = readADC(POT);
    	if (oneSecFlag){
        	fuel = fuel - ((adc_value + 1.0) / 10.0);
        	oneSecFlag = 0;
    	}
    	if (update == 1) WriteToUART(temperature, fuel);
    	WriteSpi(temperature, fuel, overheat);
		MoveServo(adc_value);
      
	}
   return 0;
}
// *****************************************************************************

void initADC( int amask)
{
	AD1PCFG = amask;    	// select analog input pins
	AD1CON1 = 0x00E0;    	// automatic conversion after sampling
	AD1CSSL = 0;        	// no scanning required
	AD1CON2 = 0;        	// use MUXA, AVss & AVdd used as vref +/-
	AD1CON3 = 0X1F01;    	// Tad=2*(1+1)*Tpb=4*100ns>Tpb(min)=135ns
	AD1CON1bits.ADON = 1;	// turn on the ADC
}

int readADC( int ch)
{
	AD1CHSbits.CH0SA = ch;	// 1. select analog input
	AD1CON1bits.SAMP = 1;	// 2. start sampling
	while (!AD1CON1bits.DONE);	// 3. wait for conversion completion
	return ADC1BUF0;    	// 4. read the conversion result   
}

void WriteSpi(double temperature, double fuel, int overheat){
	char str[16];
	unsigned char* c_buffer;
	if (overheat != 1){
    	SPI1BUF=0x1b;        	// cursor reset - first send escape char
      	c_buffer="[j";      	// command sequence for clear screen and reset cursor
       	putsSPI1(2,c_buffer);	// write out string
    	DelayMs(100);
    	sprintf(str, "Temperature: %3.1f" , temperature);

    	putsSPI1(strlen(str),str);	// write out string
    	DelayMs(100);
    	SPI1BUF=0x1b;        	// cursor reset - first send escape char
    	c_buffer="[1;0H";      	// command sequence for clear screen
    	putsSPI1(5,c_buffer);	// write out string
    	DelayMs(100);
    	sprintf(str, "Fuel level: %3.1f" , fuel);
    	putsSPI1(strlen(str),str);	// write out string
    	DelayMs(100);
	}
	else{
    	SPI1BUF=0x1b;        	// cursor reset - first send escape char
    	c_buffer="[j";      	// command sequence for clear screen and reset cursor
    	putsSPI1(2,c_buffer);
    	DelayMs(100);   // write out string
    	sprintf (str, "overheat");
    	putsSPI1(strlen(str),str);
    	DelayMs(100);
	}
}

void SPIInit(){
   
 	unsigned long int spi1mode1, spi1mode2;
	unsigned long int spi1status;
	int rData;
	SPI1CON = 0;    	// Reset SPI2 configuration register
	rData = SPI1BUF;	// clear the receive buffer  
	SPI1BRG = SPIBRG;   // set brg register value
	SPI1STATCLR=0X40;	// clear the overflow
	SPI1CON = 0x8320;	// set the mode register - this value works

	// Configure SPI2 using the c peripheral library functions
	// This PIC only has one mode register for SPI
  
	spi1mode1 = MASTER_ENABLE_ON | SPI_SMP_ON | SPI_CKE_ON |
            	SPI_MODE8_ON | SLAVE_ENABLE_ON;
	spi1mode2 = SPI_ENABLE;
	OpenSPI1(spi1mode1, spi1mode2); 

	//Set port for SPI connection on pin 1 of JB and for servo
	PORTSetPinsDigitalOut (IOPORT_D, BIT_9);

	//  LCD Display hardware reset
	PORTClearBits (IOPORT_D, BIT_9);
	DelayMs(100);
	PORTSetBits (IOPORT_D, BIT_9);
	DelayMs(500);
   
	char* c_buffer;
	SPI1BUF=0x1b;
	c_buffer = "[*";
	putsSPI1(2, c_buffer);
	DelayMs(100);
   
}

void UARTInit(){


// configure UART for communication with terminal
	unsigned long int u2mode;
	unsigned long int u2status;

	u2mode = UART_EN | UART_IDLE_CON | UART_RX_TX |
        	UART_DIS_WAKE | UART_DIS_LOOPBACK |
        	UART_DIS_ABAUD | UART_NO_PAR_8BIT | UART_EN_BCLK |
        	UART_1STOPBIT | UART_IRDA_DIS | UART_MODE_SIMPLEX |
        	UART_NORMAL_RX | UART_BRGH_SIXTEEN;
	u2status = UART_TX_PIN_LOW | UART_RX_ENABLE | UART_TX_ENABLE;

   	OpenUART2(u2mode, u2status, 64);


}

void WriteToUART(double temperature, double fuel){
	char str[80];
	sprintf(str,"Temperature: %3.1f, Fuel level: %3.1f, Running time: %d\r\n",temperature, fuel, time);
	putsUART2(str);
	DelayMs(100);
	update = 0;
}

void DeviceInit(){

	DDPCONbits.JTAGEN = 0;
 
	//On MX7 LED1 is on RG12
	//       	LED2 is on RG13
	//       	LED3 is on RG14
	//       	LED4 is on RG15
	//Set ports for onboard LEDs to outputs & claer them
	PORTSetPinsDigitalOut (IOPORT_G, BIT_12);
	PORTClearBits(IOPORT_G,BIT_12);

	//Set ports for servo pin output
	PORTSetPinsDigitalOut(IOPORT_D, BIT_1);

}

void DelayMs(int cms)
{
	int ims;
	for (ims=0; ims<cms; ims++) {
    	WriteTimer1(0);
    	while (ReadTimer1() < cntMsDelay);
	}

}
void I2CInit(){
   
	int DataSz;
	int Index;

	in_char_p = &i2c_data[0];        	// set pointer to start of array
 
	OpenI2C1(I2C_EN, I2CBRG);	// configure I2C module 2
  
	SlaveAddress = 0x4f;	// slave address of TMP3 pmod set with
                        	// jumpers JP3,JP2,JP1

	i2c_data[0] = (SlaveAddress << 1) | 0x0;	// address with write bit set
	i2c_data[1] = 0x00;    	// select register 0 on tmp3 module
	DataSz = 2;            	// transmit two bytes for register config
  
	StartI2C1();    	// send the start bit
	IdleI2C1();        	// wait to complete
 
	Index = 0;      	// transmit bytes to tmp3 module
	while( DataSz )
	{
    	MasterWriteI2C1 (i2c_data[Index++]);
    	IdleI2C1();    	// wait to complete
    	DataSz--;
      
    	if(I2C1STATbits.ACKSTAT)        //ACKSTAT is 0 on slave ack
        	break;
	}

	StopI2C1();            	// Stop I2C bus
	IdleI2C1();            	// wait to complete
}  
      
double readI2C(){

	int raw_temp, temp;
	short int b, c, rd_cnt;
	b = 0;
	c = 0;
	rd_cnt = 0;
	RestartI2C1();        	// Send the stop condition
	IdleI2C1();            	// Wait to complete
  
	MasterWriteI2C1 ((SlaveAddress << 1) | 0x01);	// transmit a read command
	IdleI2C1();            	// Wait to complete

	rd_cnt = MastergetsI2C1( 2, in_char_p, 152);	// read two bytes

	StopI2C1();            	// Stop I2C bus
	IdleI2C1();            	// wait to complete

	b = (short int) i2c_data[0];	// cast bytes to 16 bits
	c = (short int) i2c_data[1];
	raw_temp = c | (b << 8);    	// combine bytes to 9 bit result
	temp = raw_temp /256;            	// shift out ls 7 bits of 0 - degrees C
	temp = ((temp * 9)/ 5) + 32;        	// Convert to Farenheit
	return temp;
}

void ServoDelay(short int counts)
{
    	WriteTimer3(0);
    	while (ReadTimer3() < counts);
}

void TimersInit()  
{
	unsigned int tcfg1;

	unsigned int tcfg2;
	unsigned int tcfg3;
	unsigned int tcfg4;

	/* Configure Timer 1. This sets it up to count a 10Mhz with a period of 0xFFFF
	*/
	tcfg1 = T1_ON|T1_SOURCE_INT|T1_PS_1_1;
	OpenTimer1(tcfg1, 0xFFFF);

	/* Config Timer 2-3. This sets it to count 312500 Hz with a period of T2_TICK	*/
	/* Timer 2 forms the least significant word and timer 3 is the most significant	*/
	/* For 32 bit operation, use SFR of first timer for control	and use second    	*/
	/* timer for interrupt operations.                                            	*/

	/* Config Timer 1. This sets it to count 1.25 MHz with a period of 20 mSec 	*/
	tcfg2 = T2_ON|T2_SOURCE_INT|T2_PS_1_8;
	OpenTimer2(tcfg2, 0x61a8);

	/* Configure Timer 4. This sets up to count 312500 Hz clk with a preset of 0xFFFF */
	tcfg3 = T3_ON|T3_SOURCE_INT|T3_PS_1_32;
	OpenTimer3(tcfg3, 0XFFFF);

	/* Config Timer 5. This sets it to count 1.25 MHz with a period of 20 mSec 	*/
	tcfg4 = T4_ON|T4_SOURCE_INT|T4_PS_1_256;
	OpenTimer4(tcfg4, T4_TICK);
  
	/* Now enable system-wide multi-vector interrrupt handling	*/
	INTEnableSystemMultiVectoredInt();
  
	/* configure timer 5 interrupt with priority of 2	*/
	ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_2);
  
	/* Clear interrupt flag	*/
	mT4ClearIntFlag();

}

void OC2Init(){
	OpenOC2(OC_ON|OC_TIMER_MODE16|OC_TIMER2_SRC|
        	OC_PWM_FAULT_PIN_DISABLE,0X753,0X61a8);
    
	SetDCOC2PWM(0x753);      	// set duty cycle to 50%
}

void MoveServo(int adc){
	int counts1;
	counts1 = 0x380 + (short int) (adc*12);
	if (counts1 > 0xBCF) counts1 = 0xBCF;
	if (counts1 < 0x390) counts1 = 0x390;
	SetDCOC2PWM(counts1); 	// set duty cycle as set by potentiometer
}
