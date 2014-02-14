#ifndef GREENNODE_DEFS_H
#define GREENNODE_DEFS_H

#include "green_common_defs.h"

/* This .h file defines constants for nodes only*/
/***************************************************************************************
 * PORT DEFINITIONS
 */

//Definitions all PORT. Values are valid for V1 and V2
#define ADC1	      P0_0	// To read data from Multiplexor
#define ADC2_MUX0     P0_1	// ADC2 never used as ADC. MUX0 to control A input in Multiplexor
#define ADC3_MUX1     P0_2      // ADC3 never used as ADC. MUX1 to control B input in Multiplexor

#define ADC4_MUX2     P0_3      // ADC4 never used as ADC. MUX2 to control C input in Multiplexor
#define ADC5	      P0_4	// ADC5 used for 6 thermistor + 2 current clamps (used for 1 clamp)
#define ADC6	      P0_5	// ADC6 used for 6 thermistor + 2 current clamps (used for 1 clamp)
// Definitions for SPI pins
#define ADC4_SCK      P0_3
#define ADC5_MO	      P0_4
#define ADC6_MI	      P0_5

#define GN_RLED       P1_0
#define SW1           P1_0  
#define RLED_SW1      P1_0  

#define GN_GLED       P1_1
#define LED_SW2       P1_1   /* To Green LED and NMOS1 for read batterry if needed. V2 */
#define GLED_SW2      P1_1   /* V1 */

#define SS_SPI_SW3    P1_2  
#define INT           P1_2   // Interruption pin for I2C

#define SDA	      P1_6
#define SCL	      P1_7

#define DD	      P2_1	// Pin for programming
#define DDSW          P2_1  

#define DC	      P2_2	// Pin for programming


/* We can choose if PCB's LEDs shine or not in each transmmition to save  battery*/
#ifdef SHINE
#define BRIGHT SHINE
#else
#define BRIGHT 1  		/* They shine by default */
#endif

/* Transmission power */
#ifdef POWER
#define POW           POWER
#else 
#define POW           RF_POWER_P10    /* Maximum transmission power by default*/
#endif

/* GreenNode version */
#ifdef V
#define VERSION       V
#else
#define VERSION       0x00 	/* GreenNode is V1 by default */
#endif


/* For pressure sensors. Specify who is he */
#ifdef PRESS_SENSOR_NUM
#define PRESS_SENSOR    PRESS_SENSOR_NUM
#else
#define PRESS_SENSOR    0xAB
#endif

/* Current sensors available */
#ifdef      CURR_SENSORS
#define     N_SENSORS   CURR_SENSORS
#else 
#define     N_SENSORS   1 	/* 1 by default */
#endif


// Define pin directions
#define INIT_SCL()	     do { IO_DIR_PORT_PIN(1, 7, IO_OUT); SCL=1;} while (0)
#define INIT_INT()	     IO_DIR_PORT_PIN(1, 2, IO_IN)  // Pin for I2C interruptions
#define SDA_SET_INPUT()      IO_DIR_PORT_PIN(1, 6, IO_IN)
#define SDA_SET_OUTPUT()     IO_DIR_PORT_PIN(1, 6, IO_OUT)
#define SCL_SET_INPUT()      IO_DIR_PORT_PIN(1, 7, IO_IN)
#define SCL_SET_OUTPUT()     IO_DIR_PORT_PIN(1, 7, IO_OUT)
#define INIT_GN_RLED()      do { GN_RLED = 0; IO_DIR_PORT_PIN(1, 0, IO_OUT); P1SEL &= ~0x01;} while (0)
#define INIT_GN_GLED()      do { GN_GLED = 0; IO_DIR_PORT_PIN(1, 1, IO_OUT); P1SEL &= ~0x02;} while (0)


//SW1 : Sensor VCC
#define INIT_SW1()   do { RLED_SW1 = 0 ; SW1 = 0; IO_DIR_PORT_PIN(1, 0, IO_OUT); P1SEL &= ~0x01;} while (0)
//SW2 : Battery Sensor VCC
#define INIT_SW2()   do { GLED_SW2 = 0 ; IO_DIR_PORT_PIN(1, 1, IO_OUT); P1SEL &= ~0x02;} while (0)
//SW2 : Battery Sensor VCC and LED ON
#define INIT_LED_SW2()   do { LED_SW2 = 0; IO_DIR_PORT_PIN(1, 1, IO_OUT); P1SEL &= ~0x02;} while (0)

//SW3 : Select (for ADC3)
#define INIT_SW3()   do { SS_SPI_SW3 = 0 ; IO_DIR_PORT_PIN(1, 2, IO_OUT); P1SEL &= ~0x04;} while (0)

/* Macros for the Multiplexor 
*******************************/
/* 0 to 5 for sensor, 6 for battery, 7 is default and GND connected  */
#define MUX_SELECT(input)			\
do {						\
  if (input == 0) {				\
    ADC2_MUX0 = 0;				\
    ADC3_MUX1 = 0;				\
    ADC4_MUX2 = 0;				\
    break ;}					\
  else if (input == 1) {			\
    ADC2_MUX0 = 1;				\
    ADC3_MUX1 = 0;				\
    ADC4_MUX2 = 0;				\
    break ;}					\
  else if (input == 2) {			\
    ADC2_MUX0 = 0;				\
    ADC3_MUX1 = 1;				\
    ADC4_MUX2 = 0;				\
    break ;}					\
  else if (input == 3) {			\
    ADC2_MUX0 = 1;				\
    ADC3_MUX1 = 1;				\
    ADC4_MUX2 = 0;				\
    break ;}					\
  else if (input == 4) {			\
    ADC2_MUX0 = 0;				\
    ADC3_MUX1 = 0;				\
    ADC4_MUX2 = 1;				\
    break ;}					\
  else if (input == 5) {			\
    ADC2_MUX0 = 1;				\
    ADC3_MUX1 = 0;				\
    ADC4_MUX2 = 1;				\
    break ;}					\
  else if (input == 6) {			\
    ADC2_MUX0 = 0;				\
    ADC3_MUX1 = 1;				\
    ADC4_MUX2 = 1;				\
    break ;}					\
  else if (input == 7) {			\
    ADC2_MUX0 = 1;				\
    ADC3_MUX1 = 1;				\
    ADC4_MUX2 = 1;				\
    break ;}					\
  else {					\
    ADC2_MUX0 = 1;				\
    ADC3_MUX1 = 1;				\
    ADC4_MUX2 = 1;				\
  }						\
  halWait(10);					\
 } while (0)    
  
/******************************************************************************
 * STRUCT and DATA TYPES DEFINITIONS
 */

/* Next struct is defined for data packets with absolute identity byte*/
typedef struct {
  BYTE tx_header[6];
  BYTE id_byte;
  BYTE tx_rate;
  BYTE check_byte;
  UINT16 temp_measure[6*TX_EVERY];
  UINT16 humidity;
  UINT16 battery;
} temphum;

/* Next struct is defined for data packets with absolute identity byte*/
typedef struct {
  BYTE tx_header[6];
  BYTE id_byte;
  BYTE tx_rate;
  UINT16 temp_measure[6*TX_EVERY];
  UINT16 battery;
} temp;


/* Next struct is defined for data packets with absolute identity byte*/
typedef struct {
  BYTE tx_header[6];
  BYTE id_byte;
  BYTE tx_rate;
  UINT32 curr_measure[N_SENSORS*TX_EVERY];
  UINT16 battery;
} current;


/* Next struct is defined for data packets with absolute identity byte*/
typedef struct {
  BYTE tx_header[6];
  BYTE id_byte;
  BYTE tx_rate;
  BYTE press_sensor_number;
  BYTE check_byte[TX_EVERY];
  UINT16 press_measure[TX_EVERY];
  UINT16 battery;
} pressure;

   
#endif
