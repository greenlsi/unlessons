/*
 * ==================================================================================
 *       Filename:  server-parser.c
 *
 *    Description:  GreenServerIface v2 - Interface between tulio USB and 
 *                  Monitor Application
 *
 *        Version:  1.0
 *        Created:  22/04/13 00:44:43
 *       Revision:  josueportiz 15/07/13
 *       Compiler:  gcc
 *
 *         Author:  Marina Zapater (), marina@die.upm.es
 *                  Josue Portiz (), josueportiz@die.upm.es
 * =================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <math.h>
#include <inttypes.h>

#include "green_common_defs.h"
#include "server_defs.h"

/*** GLOBAL VARIABLES ***/
hwdata bat, hum, temp[6], curr[6], press;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clear_hwdata_struct
 *  Description:  frees pointers of struct and sets everything to 0
 * =====================================================================================
 */
void init_hwdata_struct (hwdata* thisdata, int size){
	
	int i=0;
	
	for (i=0; i< size; i++){
		//put all fields to zero
		memset(&thisdata[i], 0, sizeof(hwdata));
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clear_hwdata_struct
 *  Description:  frees pointers of struct and sets everything to 0
 * =====================================================================================
 */
void clear_hwdata_struct (hwdata* thisdata, int size){

	int i=0;

    //printf("DEBUG: Clearing HWdata struct: for size %d\n", size);
	for (i=0; i< size; i++){
		//free pointers
		if (thisdata[i].tstamp != NULL)
			free (thisdata[i].tstamp);
		if (thisdata[i].data != NULL)
			free (thisdata[i].data);

		//put all fields to zero
		memset(&thisdata[i], 0, sizeof(hwdata));
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  calculate_termistor_value
 *  Description:  Converts ADC thermistor value to a temperature
 * =====================================================================================
 */
float calculate_termistor_value (u_int16_t adcvalue){
       
   float vt, rt_r25, invt, tvalue;
   float a,b,c,d;
   
   //Check thermistor type
   if ( T_EC95F103WN ){
       
       /* printf("ADC- Raw value read %d\n",adcvalue); */

       vt = ((float) (adcvalue)) * VDD / ADC_RES ;

       if ( vt > (VDD - 0.1) ){
            //Nothing connected, we return 0
            /*printf("No thermistor connected\n");*/
            return 0;
       }
       /* printf("ADC - VOLTAGE VALUE READ: %f\n", vt); */
       
       rt_r25 = (vt * CURR_RDIVTERM) / (VDD - vt) /T_EC95F_R25;
       /* printf("Rt/R25 value: %f\n", rt_r25); */
       
       if ( rt_r25 > 3.274 ){
            a = 3.3538646E-03 ;
            b = 2.5654090E-04 ;
            c = 1.9243889E-06 ;
            d = 1.0969244E-07 ;
       }
       else if ( (rt_r25 > 0.36036 ) && (rt_r25 <= 3.274 ) ) {
             a =   3.3540154E-03 ; 
             b =   2.5627725E-04 ;
             c =   2.0829210E-06 ;
             d =   7.3003206E-08 ;
       }
       else if ( (rt_r25 > 0.06831 ) && (rt_r25 <= 0.36036 ) ) {
              a =  3.3539264E-03 ;
              b =  2.5609446E-04 ;
              c =  1.9621987E-06 ;
              d =  4.6045930E-08 ;
       }
       else if ( (rt_r25 > 0.01872 ) && (rt_r25 <= 0.06831 ) ) {
            a = 3.3368620E-03 ;
            b = 2.4057263E-04 ;
            c = -2.6687093E-06 ;
            d = -4.0719355E-07 ;
       }
       else {
            a=0; b=0; c=0; d=0;
            //rt_r25 has no right value
            printf("Rt/R25 invalid value\n");
            return 0;
       
       }
   
       invt = a+b*(logf(rt_r25))+c*(logf(rt_r25))*(logf(rt_r25))+d*(logf(rt_r25))*(logf(rt_r25))*(logf(rt_r25));

       //Convert to celsius 
       tvalue = 1/invt - 273.15;
       return tvalue ;
   }

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parse_tulio_battery
 *  Description:  Battery parser
 * =====================================================================================
 */
void parse_tulio_battery ( unsigned char *data, int size, time_t tstamp, unsigned char tx_every){
    
    float fbat;
    u_int16_t battery, idsensor; 
    
    u_int8_t version; /* To check GreenNode PCB version */
    version = data[6] & 0xE0;	/* PCB Version is indicated in id.byte in the 3th first bits*/

    // Id sensor
    idsensor = (data[2] << 8) | data[3] ; /* Sensor TX_ADD */

    // Resseting battery struct
    clear_hwdata_struct(&bat,1);

    //Battery
    memcpy ((char*) &battery, &data[size - (PACKET_BAT_CRC_PADDING)], 2);
    /* Headers + 1 for absolute address + 1 for TX_RATE + 1 check byte + temperature + 2 for humidity */
  
    bat.id_sensor=idsensor*SENSOR_ID_MUL + BAT_SENSOR_ID_OFFSET;
    bat.hw_parent=0;
    bat.ndata=1;

    if (version == 0x00){ /* PCB V1 */
        printf("PCB version v1 \n");
        /*0.90036 is the quotient between 2.44V (for 197 KOhm input resistance in
          ADC mode) and 2.71V ideally expected */
        fbat = (((battery * VDD / ADC_RES)/IMPEDANCE_ADJUST_V1)*BATT_RES_BRIDGE_V1)+ DIODE_VD + PMOS_VD;
    }
    else { /* PCB V2 */
        printf("PCB version v2\n");
        fbat = (((battery * VDD / ADC_RES)-V_OS_TEMP)*(4.7+5.7*RON_10K_PULL_UP_BRIDGE))+ DIODE_VD + PMOS_VD;
    }

    bat.data = realloc (bat.data, sizeof(float));
    bat.tstamp = realloc (bat.tstamp, sizeof(u_int64_t));
    
    bat.data[0] = fbat;
    bat.tstamp[0] = (u_int64_t) tstamp;
    //printf("DEBUG: Battery value = %d -- fbat = %f at tstamp %" PRIu64 "\n",
    //       battery, bat.data[0], bat.tstamp[0]);
           
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parse_tulio_temperature
 *  Description:  Fills-in temperature struct
 * =====================================================================================
 */
void parse_tulio_temperature (u_int16_t idsensor, u_int16_t *temp_measure, time_t tstamp, unsigned char tx_every){
    int i=0, j=0;
        
    for (i=0; i< 6; i++){
        temp[i].id_sensor=idsensor*SENSOR_ID_MUL+i;
        temp[i].hw_parent=idsensor;
        temp[i].ndata=tx_every;
        temp[i].data = realloc (temp[i].data, tx_every*sizeof(float));
        temp[i].tstamp = realloc (temp[i].tstamp, tx_every*sizeof(u_int64_t));

        for (j=0; j< tx_every; j++){
            temp[i].data[j] = calculate_termistor_value(temp_measure[j*6+i]);
            temp[i].tstamp[j] =  ((u_int64_t) tstamp) - (DEFAULT_SAMPLE_SECS*(tx_every-(j+1)));
            //printf("[server-parser] DEBUG: Data value -- sensorid: %d -- data[%d]: %f\n",
            //temp[i].id_sensor,j,temp[i].data[j]);
        }
    }  
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parse_tulio_data_6t1h
 *  Description:  6TERM-1H parser
 * =====================================================================================
 */
void parse_tulio_data_6t1h ( unsigned char *data, int size, time_t tstamp, unsigned char tx_every){
   
    u_int8_t *check_byte;
    u_int16_t idsensor, *humidity, *temp_measure;
  
    //DEBUG: dump buffer contents!
    /*for (i = 0; i < size; i++){ */
    /*   printf("%d - %02x\n", i, data[i]);     */
    /*} */

    // Reseting battery struct
    clear_hwdata_struct(&hum,1);
    // Reseting battery struct
    clear_hwdata_struct(temp,6);

    // Id sensor
    idsensor = (data[2] << 8) | data[3] ;    /* Sensor TX_ADD */
	
    //Temperature
    temp_measure = (u_int16_t*) &data[TULIO_TX_HEADER+1+1+1] ;  /* Headers + absolute address */
    parse_tulio_temperature(idsensor, temp_measure, tstamp, tx_every);
    
    //Parsing humidity
    //Check Byte
    check_byte = &data[TULIO_TX_HEADER+1+1];
    /* 1 Byte for humidity check. Header + absolute address + TX_RATE*/
  
    if (((*check_byte) & 0x01) == 0x01){
        printf("WARNING: ACK Humidity Request FAILED\n");
    }
    else if (((*check_byte) & 0x02) == 0x02){
        printf("WARNING: ACK Humidity Read FAILED\n");
    }
    else if (((*check_byte) & 0x0C) == 0x0C){
        printf("WARNING: Diagnostic Condition in Humidity Sensor\n");
    }
    else if (((*check_byte) & 0x0C) == 0x08){
        printf("WARNING: Humidity Sensor in Command Mode\n");
    }
    else if (((*check_byte) & 0x0C) == 0x04){
        printf("WARNING: STALE DATA in Humidity Sensor\n");
    }
    else if (((*check_byte) & 0x0C) == 0x00){
        printf("Normal Operation: Valid Humidity Value\n");
    }
    /* else{ */
    /*   fhum = 0;        	/\* Is an error *\/ */
    /* } */
  
    //Humidity    
    humidity = (u_int16_t*) &data[TULIO_TX_HEADER+1+1+1+6*tx_every*2];
    /* Headers + 1 for absolute address + 1 check byte + temperature */
    hum.id_sensor=idsensor*SENSOR_ID_MUL + I2C_SENSOR_ID_OFFSET;
    hum.hw_parent=0;
    hum.ndata=1;
  
    hum.data = realloc (hum.data, sizeof(float));
    hum.tstamp = realloc (hum.tstamp, sizeof(u_int64_t));
	
    hum.data[0] = (*humidity)*100/HUM_RES; 
    hum.tstamp[0] = (u_int64_t) tstamp; 
    //printf("DEBUG: Humidity value = %d -- hum.data[0] = %f at tstamp %" PRIu64 "\n",
    //       (*humidity), hum.data[0], hum.tstamp[0]);

    // Parse tulio battery
    parse_tulio_battery ( data, size, tstamp, tx_every );
	  
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parse_tulio_data_6t
 *  Description:  6TERM parser
 * =====================================================================================
 */
void parse_tulio_data_6t ( unsigned char *data, int size, time_t tstamp, unsigned char tx_every){

    u_int16_t idsensor;
    u_int16_t *temp_measure;
  
    //DEBUG: dump buffer contents!
    /* for (i = 0; i < size; i++){  */
    /*     printf("%d - %02x\n", i, data[i]);      */
    /* }  */

    // Reseting battery struct
    clear_hwdata_struct(temp,6);
    // Id sensor
    idsensor = (data[2] << 8) | data[3] ;    /* Sensor TX_ADD */

    //------------------------------------------------------------------------------------
    // Careful: "data" buffer has to be read in the order in which its struct is declared.
    // Struct is: temp
    //------------------------------------------------------------------------------------
  
    //Temperature
    temp_measure = (u_int16_t*) &data[TULIO_TX_HEADER+1+1] ;  /* Headers + absolute address */
    parse_tulio_temperature(idsensor, temp_measure, tstamp, tx_every);
  
    //Battery
    parse_tulio_battery ( data, size, tstamp, tx_every );
}


/******************************************************************************
 *  Current functions 
 */

float calculate_power_from_values (u_int16_t* values){
    
    float adcrms, irms, power;
    u_int16_t sample=0, lastsample;
    u_int32_t sqI, sumI=0;
    int32_t filtered = 0;
    int16_t lastfiltered;
    u_int32_t i;

    lastsample = values[0];
    lastfiltered = 0;

    //Convert to power...

    for (i=0; i<SAMPLES_PER_WAVE; i++) {
      sample = values[i];
      filtered = lastfiltered + sample - lastsample;
      filtered = 249 * filtered / 250;
      lastsample = sample;
      lastfiltered = filtered;
        
      //Square and accumulate sample
      sqI = lastfiltered * lastfiltered;
      sumI = sumI + sqI;
      
      /*printf ("-f:%d -s:%d\n", lastfiltered, sample);*/

    }

    /*printf ("Sumvalue %d (%f)\n", sumI, (1.0 * sumI)/SAMPLES_PER_WAVE);*/
    adcrms = sqrt((1.0*sumI)/SAMPLES_PER_WAVE);
    irms = (adcrms * ADC_VDD / ADC_RES) * CURR_COIL_TURNS * 1.1 / CURR_RBURDEN_ULOW;

    power = VOLTAGE * irms ;

    return power;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  calculate_power_value
 *  Description:  receives ADC current value (already squared and accumulated) 
 *                and converts to power value 
 * =====================================================================================
 */
float calculate_power_value (u_int32_t sumvalue, int type){
  
  float adcrms, irms, power, irms_corrected;
  /*power = 1.0 * sumvalue;*/
  
  //Convert to irms...
  /*printf ("Sumvalue %d (%f)\n", sumvalue, power/NUM_ADC_SAMPLES);*/
  adcrms = sqrt((1.0 * sumvalue)/NUM_ADC_SAMPLES);
  
  if (type == LOW_POWER_FLAG){
    irms = (adcrms * ADC_VDD/ADC_RES) * CURR_COIL_TURNS / CURR_RBURDEN_LOW;
  }
  else {
      //printf("[server-parser] DEBUG: calculate_power_value: using high power values\n");  
    irms = (adcrms * ADC_VDD/ADC_RES) * CURR_COIL_TURNS / CURR_RBURDEN_HIGH;
  }
  
  //Correct irms value (corrected with Fluke current clamp)
  if ( (irms > 0.0 ) && (irms < 0.49) ) {
    irms_corrected = irms + 0.05;
  }
  else if ( (irms > 0.49) && (irms < 1.3) ){
    irms_corrected = 0.8*irms + 0.124 ; 
  }
  else{
    irms_corrected = irms;
  }
  
  //Convert to power
  power = VOLTAGE * irms_corrected ;
  return power;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parse_tulio_data_curr
 *  Description:  Current packet parser
 * =====================================================================================
 */
void parse_tulio_data_curr ( unsigned char *data, int size, time_t tstamp, unsigned char tx_every, unsigned char data_type ){
  
    int i=0, j=0, powertype=LOW_POWER_FLAG;
	u_int16_t idsensor;

	int n_sensors; 	/* Number of current clamps in node */
	u_int32_t *current;

	//Calculating number of sensors (according to packet size)
	n_sensors = (size-(TULIO_TX_HEADER+PACKET_ADDR+PACKET_BAT_CRC_PADDING))/(4*tx_every);
        printf("[server-parser] Number of current sensors connected is: %d\n", n_sensors);
        
	// Reseting battery struct
	clear_hwdata_struct(curr,6);
  
	/*DEBUG: dump buffer contents!*/
	//for (i = 0; i < size; i++){
	//	printf("%d - %02x\n", i, data[i]);    
	//}
  
	idsensor = (data[2] << 8) | data[3] ;    /* Sensor TX_ADD */

        if (data_type == CURR_ID_BYTE){
            printf("[server-parser] Parsing low power current values (default)\n");
            powertype = LOW_POWER_FLAG;
        }
        else { //HIGHCURR_ID_BYTE
            printf("[server-parser] Parsing HIGH power current values\n");
            powertype = HIGH_POWER_FLAG;
        }
        
	//Current
	current = (u_int32_t*) &data[TULIO_TX_HEADER+1+1];

	for (i=0; i< n_sensors; i++){
		curr[i].id_sensor=idsensor*SENSOR_ID_MUL+i;
		curr[i].hw_parent=idsensor;
		curr[i].ndata=tx_every;
		curr[i].data = realloc (curr[i].data, tx_every*sizeof(float));
		curr[i].tstamp = realloc (curr[i].tstamp, tx_every*sizeof(u_int64_t));
		for (j=0; j< tx_every; j++){
                    curr[i].data[j] = calculate_power_value(current[j*n_sensors+i], powertype);
                    curr[i].tstamp[j] = ((u_int64_t) tstamp) - (DEFAULT_SAMPLE_SECS*(tx_every-(j+1))); 
		}
	}
  
	//Battery
	parse_tulio_battery ( data, size, tstamp, tx_every );
}


/*******************************************************************
 *  Pressure functions
 *******************************************************************/
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  average_pressure_correction
 *  Description:  corrects the pressure value for each sensor (eliminates bias)
 * =====================================================================================
 */
float average_pressure_correction(int press_sensor_number){
  float deviation = 0.0;

  if (press_sensor_number == S0027){
    deviation = D0027;
  }
  else if (press_sensor_number == S0030){
    deviation = D0030;
  }
  else if (press_sensor_number == S0033){
    deviation = D0033;
  }
  else if (press_sensor_number == S0034){
    deviation = D0034;
  }
  else if (press_sensor_number == S0040){
    deviation = D0040;
  }
  else{
    deviation = 0.0;
  }
  return deviation;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parse_tulio_data_1p
 *  Description:  parses pressure tulio packet
 * =====================================================================================
 */
void parse_tulio_data_1p ( unsigned char *data, int size, time_t tstamp, unsigned char tx_every){

	u_int8_t i=0;
	u_int16_t idsensor;
	u_int8_t  *check_byte;	  
	u_int16_t *pressure;
  
	float fpress, deviation, press_percent;
  
	u_int8_t press_sensor_number ;		/* to check pressure sensor and correct its deviation */

	// Reseting battery struct
	clear_hwdata_struct(&press, 1);

	//DEBUG: dump buffer contents!
	/* for (i = 0; i < size; i++){  */
	/*    printf("%d - %02x\n", i, data[i]);      */
	/*  }  */
  
	//Pressure sensor parameters
	idsensor = (data[2] << 8) | data[3] ;	   /* Sensor TX_ADD */
	press_sensor_number = data[8];

	//Pressure
	press.id_sensor= idsensor*SENSOR_ID_MUL + I2C_SENSOR_ID_OFFSET;
	press.hw_parent=0;
	press.ndata=tx_every;
    
	/* I2C Communication */
	
	check_byte = (u_int8_t*) &data[TULIO_TX_HEADER+1+1+1];	  
	pressure = (u_int16_t*) &data[TULIO_TX_HEADER+1+1+1+tx_every];
  
	press.data = realloc (press.data, tx_every*sizeof(float));
	press.tstamp = realloc (press.data, tx_every*sizeof(u_int64_t));
  
	for (i = 0; i < tx_every; i++) {
		//Check Byte
		if ((check_byte[i] & 0x01) == 0x01){
			printf("WARNING: ACK Pressure Request FAILED\n");
		}
		else if ((check_byte[i] & 0x02) == 0x02){
			printf("WARNING: ACK Pressure Read FAILED\n");
		}
		else if ((check_byte[i] & 0x0C) == 0x0C){
			printf("WARNING: FAULT DETECTED in Pressure Sensor\n");
		}
		else if ((check_byte[i] & 0x0C) == 0x08){
			printf("WARNING: STALE DATA in Pressure Sensor \n");
		}
		else if ((check_byte[i] & 0x0C) == 0x04){
			printf("WARNING: RESERVED Command in Pressure Sensor\n");
		}
		else if ((check_byte[i] & 0x0C) == 0x00){
			printf("Normal Operation: Valid Pressure Value\n");
		}
		else{
			fpress = 0;        	/* Is an error */
		}

		deviation = average_pressure_correction(press_sensor_number); /* Search for deviation */
    
		if (fpress != 0){ /* NO ERROR. This value !=0 is the latest value, not the newest */
      
			/* PORT 1 (RIGTH) TOP THE RAISED-FLOOR */
			/* PORT 2 (LEFT) BOTTOM THE RAISED-FLOOR */
      
			if (pressure[i] == 0){ 	/* Pressure port 1 = max  or: port 1 and port 2 are obstructed*/
				press_percent = 0.0;
			}
			else if (pressure[i] == 16383){  	/* Pressure port 2 = max  */
				press_percent = 100.0;
			}
			else { 	/* Pressure port 1 > port 2 . Expected situation */
				press_percent = (100.0*((0.9*PRESS_RANGE/ (PRESS_MAX-PRESS_MIN))*(pressure[i]- PRESS_MIN)
										+ (0.05*PRESS_RANGE))/ PRESS_RANGE) + deviation ; /* Data expressed in % */
			}
      
			press.data[i] = ((press_percent/25.0)-PRESS_SPAN)*INH2O_TO_PA; /* Data in Pascal */
	  
			printf("%f \n", press.data[i]);      
        
		}
		else{
			press.data[i] = 0;
		}
   
		press.tstamp[i] = (u_int64_t) tstamp - DEFAULT_SAMPLE_SECS*(tx_every-(i+1)); 

		/* B type Transfer Function */

	} /* End FOR */
  
	//Battery
	parse_tulio_battery ( data, size, tstamp, tx_every );  
}

void parse_tulio_data_6ctest ( unsigned char *data, int size, time_t tstamp){
    
  int i=0;
  u_int16_t samples[50];
  float power;
  
  /*DEBUG: dump buffer contents!*/
  /* for (i=0; i< C6TEST_PACKET_SIZE; i++){ */
  /*     printf("%d - %02x\n",i, data[i]);     */
  
  /* } */
  
  //Current
  for (i=0; i< 50; i++){
    memcpy ((char*) &samples[i], &data[TULIO_TX_HEADER+1+1+2*i], 2);
    printf("SAMPLE: %d\n", samples[i]);
    } 
  
  power = calculate_power_from_values (samples);
  printf ("POWER from values: %f\n", power);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parse_recv_buffer
 *  Description:  Decides packet type and sends to parser
 * =====================================================================================
 */
int parse_recv_buffer ( unsigned char *buff, unsigned char data_type, time_t ltime, unsigned char tx_every, int idx){
	
	int printing = 0;
        int expected = (TULIO_TX_HEADER + TULIO_CONFIG_BYTES);
        
	switch (data_type){
		/* In this cases, packets are identified according to ID_BYTE (6)
		   Size is sent but not used for identification purposes */
	case TH_ID_BYTE: 
		printf("Received temperature-humidity packet (ID header)\n");
                //Calculated expected size of packet
                expected += TULIO_CHECK_BYTE + 6*2*tx_every + TULIO_HUM_VALUE + PACKET_BAT_CRC_PADDING;
                if (expected != idx){
                    printf("[server-parser]Received temp-hum size does not match expected (%d). Not parsing\n",
                           expected);
                    break;
                }
		parse_tulio_data_6t1h( buff, idx, ltime, tx_every);
		printing = USED_HUM_DATA | USED_TEMP_DATA ;
		break;
	    
	case T6_ID_BYTE:
		printf("Received temperature packet (ID header)\n");
                //Calculate expected size of packet
                expected += 6*2*tx_every + PACKET_BAT_CRC_PADDING;
                if (expected != idx){
                    printf("Received temperature size does not match expected (%d). Not parsing.\n",
                           expected);
                    break;
                }
                //Parsing packet
                parse_tulio_data_6t( buff, idx, ltime, tx_every);
		printing = USED_TEMP_DATA ;
		break;
              
	case CURR_ID_BYTE:
        case HIGHCURR_ID_BYTE:    
            //Calculate expected size of packet
            expected = (idx-(TULIO_TX_HEADER+TULIO_CONFIG_BYTES+PACKET_BAT_CRC_PADDING))%(4*tx_every);
            if (expected){
                printf("Received %d current clamp packed (%d). Incorrect size. Not parsing. \n",
                       idx, expected);
                break;
            }
            //Parsing packet
            expected = (idx-(TULIO_TX_HEADER+TULIO_CONFIG_BYTES+PACKET_BAT_CRC_PADDING))/(4*tx_every);
            printf("Received %d current clamp packet (ID header)\n", expected);
            parse_tulio_data_curr( buff, idx, ltime, tx_every, data_type);
            printing = USED_CURR_DATA ;
            break;
              
	case PRESS_ID_BYTE:
            //FIXME-marina: need to perform expected size check!
		printf("Received differential pressure packet (ID header)\n");
		parse_tulio_data_1p( buff, idx, ltime, tx_every);
		printing = USED_PRES_DATA ;
		break;
              
	case CURR_TEST_ID_BYTE:
		printf("Received CURRENT TEST PACKET (ID header)\n");
		parse_tulio_data_6ctest( buff, C6TEST_PACKET_SIZE, tx_every);
		printing = USED_CURR_DATA ;
		break;

	case 0:
            if (idx == C6TEST_PACKET_SIZE) {
                printf("Received CURRENT TEST PACKET (size)n");
                parse_tulio_data_6ctest( buff, C6TEST_PACKET_SIZE, tx_every);
                printing = USED_CURR_DATA ;
            }
            else {
                printf("Unknown message type. Not parsed\n");
                break;
            }
            break;
	    	 	    
	default:
            printf("Unknown message type. Not parsed\n");
            break;
	}

	return printing;
}


