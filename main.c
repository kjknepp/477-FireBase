/*
	Simple example to open a maximum of 4 devices - write some data then read it back.
	Shows one method of using list devices also.
	Assumes the devices have a loopback connector on them and they also have a serial number

	To build use the following gcc statement
	(assuming you have the static d2xx library in the /usr/local/lib directory
	and you have created a symbolic link to it in the current dir).
	gcc -o static_link main.c -lpthread libftd2xx.a
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "/home/pi/release/examples/ftd2xx.h"

#define BUF_SIZE 32

#define MAX_DEVICES		5

FT_HANDLE ftHandle[MAX_DEVICES];
char * 	pcBufRead = NULL;
char * 	pcBufLD[MAX_DEVICES + 1];
char 	cBufLD[MAX_DEVICES][64];
DWORD	dwRxSize = 0;
//DWORD 	dwBytesWritten;
DWORD	dwBytesRead;
FT_STATUS	ftStatus;
int	iNumDevs = 0;
int	i;
int	iDevicesOpen;

int hash;
int rxid;
int txid;
int opcode;
int data;
short temperature;
short humidity;
int pressure;


int c_init(void);
void c_close(void);
void usb_receive(void);
int gethash(void);
int getrxid(void);
int gettxid(void);
int getopcode(void);
int getdata(void);
short gettemperature(void);
short gethumidity(void);
int getpressure(void);




int main()
{
	printf("Something\n");

	for(i = 0; i < MAX_DEVICES; i++) {
		pcBufLD[i] = cBufLD[i];
	}
	pcBufLD[MAX_DEVICES] = NULL;

	ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);

	if(ftStatus != FT_OK) {
		printf("Error: FT_ListDevices returned %d\n", (int)ftStatus);
		return 1;
	}

	for(i = 0; ( (i <MAX_DEVICES) && (i < iNumDevs) ); i++) {
		printf("Device %d Serial Number - %s\n", i, cBufLD[i]);
	}


	for(i = 0; ( (i <MAX_DEVICES) && (i < iNumDevs) ) ; i++) {
		/* Setup */
		if((ftStatus = FT_OpenEx(cBufLD[i], FT_OPEN_BY_SERIAL_NUMBER, &ftHandle[i])) != FT_OK){
			/*
				This can fail if the ftdi_sio driver is loaded
		 		use lsmod to check this and rmmod ftdi_sio to remove
				also rmmod usbserial
		 	*/
			printf("Error: FT_OpenEx returned %d for device %d\n", (int)ftStatus, i);
			return 1;
		}

		printf("Opened device %s\n", cBufLD[i]);

		if((ftStatus = FT_SetBaudRate(ftHandle[i], 38400)) != FT_OK) {
			printf("Error: FT_SetBaudRate returned %d, cBufLD[i] = %s\n", (int)ftStatus, cBufLD[i]);
			break;
		}


		//cBufWrite = "Hello World!!!!!";

		/* Write */
		//printf("Writing\n");
		//ftStatus = FT_Write(ftHandle[i], "Hello World!!!!!", BUF_SIZE, &dwBytesWritten);
		//printf("Done Writing\n");
		//if(ftStatus != FT_OK) {
		//	printf("Error: FT_Write returned %d\n", (int)ftStatus);
		//	break;
		//}
		//sleep(1);

		/* Read */


		int j;
		for(j=0; j<20; j++){
			//sleep(1);

			dwRxSize = 0;
			while ((dwRxSize < BUF_SIZE) && (ftStatus == FT_OK)) {
				ftStatus = FT_GetQueueStatus(ftHandle[i], &dwRxSize);
			}
		//if(ftStatus == FT_OK) {
			pcBufRead = (char*)realloc(pcBufRead, dwRxSize);
			if((ftStatus = FT_Read(ftHandle[i], pcBufRead, BUF_SIZE, &dwBytesRead)) != FT_OK) {
				printf("Error: FT_Read returned %d\n", (int)ftStatus);
			}
			else {
				printf("FT_Read read %d bytes\n", (int)dwBytesRead);
				int k;
				for(k=0; k<32; k++){
					printf("%c", pcBufRead[k]);
				}
				printf("\n");

				hash = (pcBufRead[0]<<24) | (pcBufRead[1]<<16) | (pcBufRead[2]<<8) | pcBufRead[3];
				rxid = (pcBufRead[4]<<24) | (pcBufRead[5]<<16) | (pcBufRead[6]<<8) | pcBufRead[7];
				txid = (pcBufRead[8]<<24) | (pcBufRead[9]<<16) | (pcBufRead[10]<<8) | pcBufRead[11];
				opcode = pcBufRead[12];

				data = (pcBufRead[13]<<24) | (pcBufRead[14]<<16) | (pcBufRead[15]<<8) | pcBufRead[16];
				temperature = (pcBufRead[17]<<8) | pcBufRead[18];
				humidity = (pcBufRead[19]<<8) | pcBufRead[20];
				pressure = (pcBufRead[21]<<24) | (pcBufRead[22]<<16) | (pcBufRead[23]<<8) | pcBufRead[24];

				printf("Hash: %d\n", hash);
				printf("RXID: %d\n", rxid);
				printf("TXID: %d\n", txid);
				printf("OPCode: %d\n", opcode);
				printf("Data: %d\n", data);
				printf("Temp: %d\n", temperature);
				printf("Humidity: %d\n", humidity);
				printf("Pressure: %d\n", pressure);



				//printf("%d\n", pressure);
				if(temperature/10 > 30){

					//system("python /home/pi/Documents/477grp11/Raspberry-Pi-Email-System/pi_emailer.py");
				}

				//printf("%hu\n", (pcBufRead[17]<<8) | pcBufRead[18]);
			}
		//}
		//else {
		//	printf("Error: FT_GetQueueStatus returned %d\n", (int)ftStatus);
		//}

		}
	}

	iDevicesOpen = i;
	/* Cleanup */
	for(i = 0; i < iDevicesOpen; i++) {
		FT_Close(ftHandle[i]);
		printf("Closed device %s\n", cBufLD[i]);
	}

	if(pcBufRead)
		free(pcBufRead);
	return 0;
}



int c_init(void){
	//char 	cBufWrite[BUF_SIZE];

	//printf("Something\n");

	for(i = 0; i < MAX_DEVICES; i++) {
		pcBufLD[i] = cBufLD[i];
	}
	pcBufLD[MAX_DEVICES] = NULL;

	ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);

	if(ftStatus != FT_OK) {
		//printf("Error: FT_ListDevices returned %d\n", (int)ftStatus);
		return 1;
	}

	for(i = 0; ( (i <MAX_DEVICES) && (i < iNumDevs) ); i++) {
		//printf("Device %d Serial Number - %s\n", i, cBufLD[i]);
	}


	if((ftStatus = FT_OpenEx(cBufLD[i], FT_OPEN_BY_SERIAL_NUMBER, &ftHandle[0])) != FT_OK){
		/*
			This can fail if the ftdi_sio driver is loaded
 			use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
	 	*/
		//printf("Error: FT_OpenEx returned %d for device %d\n", (int)ftStatus, i);
		return 1;
	}

		//printf("Opened device %s\n", cBufLD[i]);

	if((ftStatus = FT_SetBaudRate(ftHandle[0], 9600)) != FT_OK) {
		//printf("Error: FT_SetBaudRate returned %d, cBufLD[i] = %s\n", (int)ftStatus, cBufLD[i]);
		return 1;
	}
	return 0;
}
void c_close(void){
	FT_Close(ftHandle[0]);
	//printf("Closed device %s\n", cBufLD[i]);
}
void usb_receive(void){
	/* Read */
	dwRxSize = 0;
	while ((dwRxSize < BUF_SIZE) && (ftStatus == FT_OK)) {
		ftStatus = FT_GetQueueStatus(ftHandle[0], &dwRxSize);
	}
	//if(ftStatus == FT_OK) {
	pcBufRead = (char*)realloc(pcBufRead, dwRxSize);
	if((ftStatus = FT_Read(ftHandle[0], pcBufRead, BUF_SIZE, &dwBytesRead)) != FT_OK) {
		//printf("Error: FT_Read returned %d\n", (int)ftStatus);
	} else {
		//printf("FT_Read read %d bytes\n", (int)dwBytesRead);
		//int k;
		//for(k=0; k<32; k++){
		//	printf("%c", pcBufRead[k]);
		//}
		//printf("\n");

		hash = (pcBufRead[0]<<24) | (pcBufRead[1]<<16) | (pcBufRead[2]<<8) | pcBufRead[3];
		rxid = (pcBufRead[4]<<24) | (pcBufRead[5]<<16) | (pcBufRead[6]<<8) | pcBufRead[7];
		txid = (pcBufRead[8]<<24) | (pcBufRead[9]<<16) | (pcBufRead[10]<<8) | pcBufRead[11];
		opcode = pcBufRead[12];

		data = (pcBufRead[13]<<24) | (pcBufRead[14]<<16) | (pcBufRead[15]<<8) | pcBufRead[16];
		temperature = (pcBufRead[17]<<8) | pcBufRead[18];
		humidity = (pcBufRead[19]<<8) | pcBufRead[20];
		pressure = (pcBufRead[21]<<24) | (pcBufRead[22]<<16) | (pcBufRead[23]<<8) | pcBufRead[24];

		//printf("Hash: %d\n", hash);
		//printf("RXID: %d\n", rxid);
		//printf("TXID: %d\n", txid);
		//printf("OPCode: %d\n", opcode);
		//printf("Data: %d\n", data);
		//printf("Temp: %d\n", temperature);
		//printf("Humidity: %d\n", humidity);
		//printf("Pressure: %d\n", pressure);
	}
}
int gethash(void){
	return hash;
}
int getrxid(void){
	return rxid;
}
int gettxid(void){
	return txid;
}
int getopcode(void){
	return opcode;
}
int getdata(void){
	return data;
}
short gettemperature(void){
	return temperature;
}
short gethumidity(void){
	return humidity;
}
int getpressure(void){
	return pressure;
}
