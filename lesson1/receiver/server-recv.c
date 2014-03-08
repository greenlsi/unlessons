/*
 * ==================================================================================
 *       Filename:  server-recv.c
 *
 *    Description:  GreenServerIface v2 - Interface between tulio USB and 
 *                  Monitor Application
 *
 *        Version:  1.0
 *        Created:  22/04/13 00:44:43
 *       Revision:  josueportiz 15/07/13. Several changes
 *       Compiler:  gcc
 *
 *         Author:  Marina Zapater (), marina@die.upm.es
 * =================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <inttypes.h>

#include "green_common_defs.h"
#include "server_defs.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  synch_signal
 *  Description:  Reception of SIGUSR1 indicates that information needs to be
 *                copied to output file
 * =====================================================================================
 */
volatile sig_atomic_t copy_files = 0;
volatile sig_atomic_t rsync_signal = 0;

void synch_signal (int sig)
{
    if (!rsync_signal)
       copy_files = 1;
    else
       printf("\x1B[31mResource in use for a manual rsync. Not synchronized\x1B[0m\n"); 
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  manual_rsync_signal
 *  Description:  Reception of SIGUSR2 indicates that information needs to be
 *                copied to output file
 * =====================================================================================
 */
void manual_rsync_signal (int sig)
{
   if (!copy_files) 
      rsync_signal = 1;
   else 
      printf("\x1B[31mResource in use for an automatic rsync. Not synchronized manually\x1B[0m\n"); 
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parse_arguments
 *  Description:  Parsing command-line arguments 
 * =====================================================================================
 */
void parse_arguments (int argc, char **argv)
{
    int i;

    for(i=1; i < argc; i++) {
        /*printf("Parsing command-line arguments: arg:%d - %s \n", i, argv[i]);*/
        if (strcmp(argv[i], "--help") == 0) {
           printf("\x1B[31mUsage: %s --hwfile=<hardware sensor data file> --envfile=<file for environmental sensor data> --tempfolder=<folder of session>\x1B[0m\n",argv[0]);
           exit(0);
        } 
        else if ((argc == 3) && (! strncmp(argv[i], "--hwfile=", 9))) {
           hwsensor_file = argv[i]+9;
           printf("\x1B[33m[WSN server-recv] Hardware file is: %s \x1B[0m\n", hwsensor_file);
        }
        else if (! strncmp(argv[i], "--envfile=", 10) ) {
           envsensor_file = argv[i]+10;
           printf("\x1B[33m[WSN server-recv] Environmental file is: %s \x1B[0m\n", envsensor_file);
        }
        else if (! strncmp(argv[i], "--tempfolder=", 13)){
           tempfolder = argv[i]+13;
           printf("\x1B[33m[WSN server-recv] Folder with temporary files is: %s \x1B[0m\n", tempfolder);
        }
    }
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  Main behavior 
 * =====================================================================================
 */

int main(int argc, char *argv[]) {
    // Variables
    int sock_descriptor, conn_desc, size, n, idx, i,j,k;
    int printing = 0;
    struct sockaddr_in serv_addr, client_addr;
    unsigned char auxbuff[50];
    unsigned char buff[MAX_MSG_BYTES];
    time_t ltime;
    struct tm *Tm;
    struct sigaction usr_action;  // To synchronize data automatically
    struct sigaction rsync_manual_action; // to synchronize data manually
    char copychar[200];
    char hwaux[100];
    char envaux[100];  // Buffers to store directories of temporary files
    FILE *hwfile, *envfile ;

    unsigned char  data_type; 	/* Identifying absolute addressing*/
    unsigned char  tx_every; 	


    // Parsing command-line arguments
    parse_arguments (argc, argv);

	// Initialization structs
	init_hwdata_struct (&bat,1);
	init_hwdata_struct (&hum,1);
	init_hwdata_struct (temp,6);
	init_hwdata_struct (curr,6);
	init_hwdata_struct (&press,1);

    // Signal handling
    sigset_t block_mask;

    sigfillset (&block_mask);
    usr_action.sa_handler = synch_signal;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;
    sigaction (SIGUSR1, &usr_action, NULL);

    // Signal handling for synch_signal
    rsync_manual_action.sa_handler = manual_rsync_signal;
    rsync_manual_action.sa_mask = block_mask;
    rsync_manual_action.sa_flags = 0;
    sigaction (SIGUSR2, &rsync_manual_action, NULL);


    // Socket handling
    sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if(sock_descriptor < 0){
        printf("\x1B[31m[WSN server-recv] Failed creating socket\x1B[0m\n");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    serv_addr.sin_port = htons(PORT);

    if (bind(sock_descriptor, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        printf("\x1B[31m[WSN server-recv] Failed to bind\x1B[0m\n");

    listen(sock_descriptor, 5);
    
    printf("\x1B[33m[WSN server-recv] Waiting for connection...\x1B[0m\n");

    size = sizeof(client_addr);
    conn_desc = accept(sock_descriptor, (struct sockaddr *)&client_addr, &size);             

    if (conn_desc == -1){
        printf("\x1B[31m[WSN server-recv] Failed accepting connection \x1B[0m\n");
		// Program should always close all sockets (the connected one as well as the listening one)
        // as soon as it is done processing with it
        close(sock_descriptor); 
        /* Closing files */
        if (argc == 4) {
          fclose(hwfile); 
        }
        fclose(envfile);  

	return -1;
    }
    else
        printf("\x1B[32m[WSN server-recv] Connected \x1B[0m \n");

    fflush(stdout);

    /**************************************************************************************
     * Opening output files
     */
    /* Crear los ficheros auxiliares */
    sprintf(hwaux, "%s/hwsensor_auxfile.txt", tempfolder);
    sprintf(envaux, "%s/envsensor_auxfile.txt", tempfolder);
    /********************************/
    if (argc == 4){
      hwfile = fopen(hwaux, "a+");    
      if (hwfile == NULL){
        printf("\x1B[31mError opening hwfile: %s \x1B[0m\n",strerror(errno));
        return -1;
      } 
    }
    envfile = fopen(envaux, "a+");    
    if (envfile == NULL){
      printf("\x1B[31mError opening envfile: %s \x1B[0m\n",strerror(errno));
        return -1;
    } 
     


    /******************************************************************************
    * Read from descriptor and print to stdout
    */

    idx = 0;
    memset (buff, 0, sizeof(buff)); /* Stuffing "buff" with "0" up to its size */
        
    while (1) {
        n = read(conn_desc, auxbuff, sizeof(auxbuff)-1);
        
        // Only one synchronization (manual or auto) at time.
        if ((copy_files && !rsync_signal) || (!copy_files && rsync_signal)){
            
            if (argc == 4){ 	/* Current clamps installed */
                fflush(hwfile); fclose(hwfile);
                /* copy the file "hwaux" into "hwsensor_file" */
                sprintf(copychar, "cp %s %s", hwaux, hwsensor_file);
                
                printf("%s \n", copychar);
                system(copychar);
                hwfile = fopen(hwaux, "a+");
            }

            /* Copying environmental files */
            fflush(envfile); fclose(envfile);
            sprintf(copychar, "cp %s %s", envaux, envsensor_file);
            printf("%s \n", copychar);
            system(copychar);  
            envfile = fopen(envaux, "a+");  
      
            /* Restoring FLAGS */
            if (copy_files)
                copy_files = 0;
            if (rsync_signal)
                rsync_signal = 0;
	
            if (n < 0){
                continue ;        
            }    
        }
      
        if ( n > 0 ){
            if ( (idx + n) > MAX_MSG_BYTES){
                printf("\x1B[31m[WSN server-recv] ERROR: MAXIMUM BUFFER SIZE RECEIVED. DATA IGNORED\x1B[0m\n");
                fflush(stdout);
                idx=0;
                continue;
            }
	
            memcpy ( &buff[idx], auxbuff, n);
            idx+=n ;
        
            if (idx > 1) {
                if( ( buff [idx-1] == '\n') && (buff [idx-2] == '\r') ){
                    //Tx ended
                    ltime=time(NULL);
                    Tm=localtime(&ltime);
            
                    printf("-------------------------------------------------------\n");
                    printf("[WSN server-recv] %02d-%02d-%02d %02d:%02d:%02d\n", Tm->tm_mday, Tm->tm_mon+1,
                           Tm->tm_year+1900, Tm->tm_hour, Tm->tm_min, Tm->tm_sec); 
                    printf("[WSN server-recv] Received - Packet size: %d\n", idx); 

                    /* Type of sensor is defined in id_byte. See ~/include/greennode_defs.h
                       Only de 5th latest bits*/
                    data_type = buff[ID_TYPE_BYTE] & 0x1F ;
            
                    tx_every = buff[TX_EVERY_BYTE] ;
	  	    
                    /* printf("\x1B[31mbuff[6] is: %02x \x1B[0m\n", buff[6]);  */
                    /* printf("\x1B[31mData type is: %d \x1B[0m\n", data_type);  */
	    
                    /* printf("\x1B[33mtx_every is: %d \x1B[0m\n", tx_every);  */
					
                    printing = parse_recv_buffer(buff, data_type, ltime, tx_every, idx);
                    printf ("Packet parsed\n");
                    
                    // Printing information to log files: "idsensor,data,tstamp"
                    //-----------------------------------------------------------------
                    if (printing){

                        // Printing temperature data
                        if ( printing & USED_TEMP_DATA ){
                            printf("Printing temperature data...\n");
                            for (k=0; k< 6; k++){
                                for (j=0; j< tx_every; j++){
                                    if (temp[k].data[j] > 0){
                                        printf("\tPrinting temp[%d].data[%d] = %f \n", k,j, temp[k].data[j]);
                                        fprintf(envfile, "%d,%.2f,", temp[k].id_sensor, temp[k].data[j]);
                                        fprintf(envfile, "%" PRIu64 "\n", temp[k].tstamp[j]);
                                    }
                                }
                            }
                        }
                        
                        //Printing humidity data
                        if ( printing & USED_HUM_DATA ){
                            fprintf(envfile, "%d,%f,", hum.id_sensor, hum.data[0]);
                            fprintf(envfile, "%" PRIu64 "\n", hum.tstamp[0]);    
                        }

                        //Printing current data
                        if ( printing & USED_CURR_DATA ){
                            for (k=0; k< 6; k++){
                                for (j=0; j< tx_every; j++){
                                    if (curr[k].data[j] > 0) { 
                                        fprintf(hwfile, "%d,%f,", curr[k].id_sensor, curr[k].data[j]);
                                        fprintf(hwfile, "%" PRIu64 "\n", curr[k].tstamp[j]); 
                                    }
                                }
                            }
                        }

                        // Printing pressure data
                        if ( printing & USED_PRES_DATA ){
                            for (j=0; j< tx_every; j++){
                                fprintf(envfile, "%d,%d,", press.id_sensor, (int) (press.data[j])*10);
                                fprintf(envfile, "%" PRIu64 "\n", press.tstamp[j]);
                            }
                        }
                        
                        //Printing battery data (we always print battery)
                        printf("Printing battery data\n");
                        fprintf(envfile, "%d,%f,", bat.id_sensor, (bat.data[0]));
                        printf("Battery[%d] = %f \n", bat.id_sensor, bat.data[0]);
                        fprintf(envfile, "%" PRIu64 "\n", bat.tstamp[0]);
                        fflush(envfile);
                    }
                    idx = 0 ;
                    memset (buff, 0, sizeof(buff));
                    printf("-------------------------------------------------------\n");
                    fflush(stdout);
                }
            }
        }
        else {
            printf("\x1B[31m\nERROR RECEIVING \x1B[0m\n");
            break;
        }
      
    }
    // Program should always close all sockets (the connected one as well as the listening one)
    // as soon as it is done processing with it
    
    close(sock_descriptor); 
    /* Closing files */
    if (argc == 4) {
      fclose(hwfile); 
    }
    fclose(envfile);  
    
    return 0;
    
}

/*==== END OF FILE ==========================================================*/
/* vim: set tabstop=4 autoindent sw=4 smartindent fdm=syntax: */

