/***********************************************************************************
  Filename:     6term.c

  Description:  GreenNode v1 - Board for 6 thermistors (ADCs)  
                (No humidity sensor) (I2C)

  Comments:     No routing protocol
                Tulio wakes up every 5-10 seconds, collects information and sleeps
                Every minute, tulio tx to base station

***********************************************************************************/



/*==== DECLARATION CONTROL ===================================================*/
/*==== INCLUDES ==============================================================*/

#include "hal_main.h"
#include "libcc1110.h"
#include "greennode_defs.h"


//******** RF configuration************************
//these three variables MUST ALWAYS be defined*****
//*************************************************
UINT8 rf_power = POW; // TX Power Output
UINT8 rf_channel = CHANNEL; // RF channel configuration
network_address netAdd; // network address structure

//********SWITCHES CONFIGURATION********************
#define     SELECT          SS_SPI_SW3

/* Leds init for GreenNode V1 */
void leds_init_V1(void){
  INIT_GN_GLED();
  GN_GLED = 1;
  sleep(1);
  GN_GLED = 0;
  
  INIT_SW1();
  RLED_SW1 = 1;    // Retrieve current from sensors
  
  INIT_SW2();
  GLED_SW2 = 0;
  
  INIT_SW3();
  SELECT = 0; 
}

/* Leds init for GreenNode V2 */
void leds_init_V2(void){
  INIT_LED_SW2();   /* Define LED_SW2 port as output */
  LED_SW2 = 1;	    /* LED=ON */
  sleep(1);
  LED_SW2 = 0;      /* LED=OFF */
  
  INIT_SW1();	    /* Define SW1 port as output */
  SW1 = 1;          /* Remove current to sensors. PMOS1=OFF */
}

void adcs_init_V1(void){
  /* Configure MODE as TRISTATE, 197 KOhm input resistance in sampling (mode ADC ON)*/
  IO_IMODE_PORT_PIN(0, 0, IO_IMODE_TRI);
  IO_IMODE_PORT_PIN(0, 1, IO_IMODE_TRI);
  IO_IMODE_PORT_PIN(0, 2, IO_IMODE_TRI);
  IO_IMODE_PORT_PIN(0, 3, IO_IMODE_TRI);
  IO_IMODE_PORT_PIN(0, 4, IO_IMODE_TRI);
  IO_IMODE_PORT_PIN(0, 5, IO_IMODE_TRI);
}

void adcs_init_V2(void){
  /* Configure MUX selector pins */
  IO_DIR_PORT_PIN(0, 1, IO_OUT); /* MUX selector pin A */
  IO_DIR_PORT_PIN(0, 2, IO_OUT); /* MUX selector pin B */
  IO_DIR_PORT_PIN(0, 3, IO_OUT); /* MUX selector pin C */ 
  
  /* Configure MODE as TRISTATE, 197 KOhm input resistance in sampling (mode ADC ON)*/
  IO_IMODE_PORT_PIN(0, 0, IO_IMODE_TRI);
  IO_IMODE_PORT_PIN(0, 4, IO_IMODE_TRI);
  IO_IMODE_PORT_PIN(0, 5, IO_IMODE_TRI);
}



/* Reading data from Multiplexor */
UINT16 read_sensor_mux(){
  UINT16 value;
  halWait(100);
  value = halAdcSampleSingle(ADC_REF_AVDD, ADC_12_BIT, ADC_AIN0);
  return value;
}

   
/*
 * MAIN!!
 */
int
main(void)
{
  temp databuf;
  UINT8 i=0;
  BYTE status = 0, version = 0; 
  UINT16 sample_time = 0; 	/* This variable allows to sample time be reconfigurable */
  sample_time = SAMPLE_TIME;

  /* Some initializations */
  for (i=0; i<6*TX_EVERY; i++){
    databuf.temp_measure[i]=0;
  }
 
  version = VERSION << 5;  	/* PCB GreenNode Version */

  /* GreenNode Leds & ADCs Init */
  /* ADCs 
   * It is not necessary Configure ports as input
   *  they are inputs by default.
   * Example of Port direction configuration:
   * ADC_ENABLE_CHANNEL(ADC_AIN0); // Configure P0_0 as ADC input */
  
  if (version == 0x00){ 	/* GreenNode V1 */
    leds_init_V1();
    adcs_init_V1();
  } 
  else { 			/* GreenNode V2 */
    leds_init_V2();
    adcs_init_V2();
  }
  
  /***********************************************************
   * This two instructions must be together and at the begining
   * of every app. 
   */
  SET_MAIN_CLOCK_SOURCE(CRYSTAL); // Choose the crystal oscillator as the system clock
  cc1110_init(); // AWD HW init
  
  // Enable Watch Dog with 1 second period
  enable_wdt (SEC_1);         
  
  // Configure RF address
  netAdd.my_address = TX_ADD;
  
  
  /***********************************************************
   * Main loop
   */
  while (1) {
    for (i=0; i< TX_EVERY; i++){

      /*******************************************************************
       * SLEEP TULIO
       */
      // Completely Power OFF Radio
      reset_radio();
      //MAKE TULIO SLEEP - Change to Power Mode 2
      set_idle_mode_ST(sample_time, POWER_MODE_2);
      // When coming from Low Power Mode, CRYSTAL must be again chosen as System Oscillator Source
      SET_MAIN_CLOCK_SOURCE(CRYSTAL);
      // Reconfiguración de la radio: Esto siempre tiene que ir aquí 
      rf_settings(); 


            /*******************************************************************
       * RETRIEVE DATA FROM SENSORS
       ******************************************************************/
      if (version == 0x00){ 	/* GreenNode V1 */
	RLED_SW1 = 0; 	//Give current to sensors
	halWait(40);	/* This halWait() must be greater than 40 ms to ensure that the maximum value (3.3 V) is achieved when A THERMISTOR IS NOT CONECTED*/

	//READ ADC0
	while(0);while(0); 
	databuf.temp_measure[6*i] = halAdcSampleSingle(ADC_REF_AVDD,  ADC_12_BIT, ADC_AIN0); 
	while(0); while(0); 
	//READ ADC1 
	databuf.temp_measure[6*i+1] = halAdcSampleSingle(ADC_REF_AVDD,  ADC_12_BIT, ADC_AIN1);
	while(0); while(0);
	//READ ADC2 
	databuf.temp_measure[6*i+2] = halAdcSampleSingle(ADC_REF_AVDD,  ADC_12_BIT, ADC_AIN2); 
	while(0); while(0); 
	//READ ADC3 
	databuf.temp_measure[6*i+3] = halAdcSampleSingle(ADC_REF_AVDD,  ADC_12_BIT, ADC_AIN3); 
	while(0); while(0); 
	//READ ADC4 
	databuf.temp_measure[6*i+4] = halAdcSampleSingle(ADC_REF_AVDD,  ADC_12_BIT, ADC_AIN4); 
	while(0); while(0); 
	//READ ADC5 
	databuf.temp_measure[6*i+5] = halAdcSampleSingle(ADC_REF_AVDD,  ADC_12_BIT, ADC_AIN5); 
	while(0); while(0); 
	
	//Retrieve current from sensors
	RLED_SW1 = 1;

      }
      else {			/* GreenNode V2 */
      	SW1 = 0;	//Give current to sensors
      	halWait(25);
	      
      	/* Sensor 1 */
      	MUX_SELECT(0); 	/* Call function to select input 0 */
      	databuf.temp_measure[6*i] = read_sensor_mux();
      	/* Sensor 2 */
      	MUX_SELECT(1); 	/* Call function to select input 1 */
      	databuf.temp_measure[6*i+1] = read_sensor_mux();
      	/* Sensor 3 */
      	MUX_SELECT(2); 	/* Call function to select input 2 */
      	databuf.temp_measure[6*i+2] = read_sensor_mux();
      	/* Sensor 4 */
      	MUX_SELECT(3); 	/* Call function to select input 3 */
      	databuf.temp_measure[6*i+3] = read_sensor_mux();
      	/* Sensor 5 */
      	MUX_SELECT(4); 	/* Call function to select input 4 */
      	databuf.temp_measure[6*i+4] = read_sensor_mux();
      	/* Sensor 6 */
      	MUX_SELECT(5); 	/* Call function to select input 5 */
      	databuf.temp_measure[6*i+5] = read_sensor_mux();
      } 
    } //End FOR 
    
    
    /* BATTERY */
    //Read battery value 
    if (version == 0x00){ 	/* GreenNode V1 */
      GLED_SW2 = 1; 
      
      SELECT = 1;  
      halWait(100);
      databuf.battery = halAdcSampleSingle(ADC_REF_AVDD,  ADC_12_BIT, ADC_AIN2); 
      SELECT = 0;  
      GLED_SW2 = 0;
    } 
    else { 			/* GreenNode V2 */
      LED_SW2 = 1; 		/* Read from battery */
       
      halWait(100);
      MUX_SELECT(6); /* Select the battery entry */
      /* 60 ms. necessary  time to battery voltage raise up to the top in max charge */
      databuf.battery = read_sensor_mux(); /* Read data from multiplexor's output */
      MUX_SELECT(7); /* Select GND */
      
      SW1 = 1;          /* Remove current to sensors. PMOS1=OFF */
      LED_SW2 = 0;
    }
    
    /* Identity byte */
    /* If an absolute address is defined, then an identity byte is added
       in header. This byte shows us what type of information is sent.
       Types of Identities bytes are defined in ../include/green_common_defs.h 
       Also in this byte, in 3th first bits PCB version is indicated*/
    
    if (ABSOLUTE_ADDR){  	/* Absolute address or not */
      databuf.id_byte = version | (T6_ID_BYTE & 0x1F);
    }
    else {
      databuf.id_byte = version | 0x00 ;
    }

    /* TX_EVERY is sent to base */
    databuf.tx_rate = TX_EVERY;
   
    
    /*******************************************************************
     * TX to base
     */
    
    // Send Data and keep TX result in status variable
    // cc1110_tx(UINT16 src, UINT16 dst, BYTE* Buffer_add, BYTE data_len, BYTE com_type)
    status = cc1110_tx(netAdd.my_address, RX_ADD, (BYTE*) &databuf, 1+1+6*TX_EVERY*2+2, CO);
    
    // Switch leds depending on transmision status
    if (BRIGHT != 0) { 		/* In power saving mode */
      switch (status) {
      case TX_OK:
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 0;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 0;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 0;
	halWait(50);
	break;
	//default: none
	
      case TX_FAIL:
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 0;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 0;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	break;
	
      case TX_FAIL_CONFIG:
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 0;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 0;
	halWait(50);
	break;
	
      case TX_FAIL_ACK: 
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 0;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	break;

      case TX_FAIL_CHANNEL_BUSY:
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 0;
	halWait(50);
	GN_GLED = 1;
	halWait(50);
	GN_GLED = 0;
	halWait(50);
	break;
      }
      GN_GLED = 0; 		/* Whatever the status, bring LED OFF*/
    }
  }
}

//#pragma vector=ST_VECTOR
//__interrupt void stIrq(void)
// Interrupt neede for POWER_MODE_2 
void stIrq(void) __interrupt(ST_VECTOR)
{
        IRCON &= 0x7F;
        WORIRQ &= 0xFE;
        SLEEP &= 0xFC;
}


/*==== END OF FILE ==========================================================*/
/* vim: set tabstop=4 autoindent sw=4 smartindent fdm=syntax: */





