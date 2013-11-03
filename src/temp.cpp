//Include ArduPi library
#include "arduPi.h"
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAXTIMINGS 100

//#define DEBUG

#define DHT11 11

char QUERY[] = {0x7E,0x00,0x0F,0x17,0x05,0x00,0x13,0xA2,0x00,0x40,0xA8,0xBB,0x28,0xFF,0xFE,0x02,0x49,0x53,0xC8};
char INPUTP[] = {0x7E,0x00,0x10,0x17,0x05,0x00,0x13,0xA2,0x00,0x40,0xA8,0xBB,0x28,0xFF,0xFE,0x02,0x44,0x34,0x03,0xE9};
char LOWP[] = {0x7E,0x00,0x10,0x17,0x05,0x00,0x13,0xA2,0x00,0x40,0xA8,0xBB,0x28,0xFF,0xFE,0x02,0x44,0x34,0x04,0xE8};
char HIGHP[] = {0x7E,0x00,0x10,0x17,0x05,0x00,0x13,0xA2,0x00,0x40,0xA8,0xBB,0x28,0xFF,0xFE,0x02,0x44,0x34,0x05,0xE7};
char BUFFER[25] = {0};
int b = 0;

void setup(){
	Serial.begin(38400);
}

void loop(){
	printf("HIGH\n");
	for(int i=0;i<20;i++)
	{
		Serial.write(HIGHP[i]);
	}
	delay(100);
	printf("LOW\n");
	for(int i=0;i<20;i++)
	{
		Serial.write(LOWP[i]);
	}
	delay(100);

}

int main (){
	setup();
	float temp_p=0,hum_p=0;
	int counter = 0;
	int laststate = 1;
	int k = 0;
	int j = 0;
	int checksum = 0;
#ifdef DEBUG
	int bitidx = 0;
	int bits[250];
#endif
	int data[100];

	printf("HIGH\n");
	for(int i=0;i<20;i++)
	{
		Serial.write(HIGHP[i]);
	}
	delay(500);  // 500 ms

	printf("LOW\n");
	for(int i=0;i<20;i++)
	{
		Serial.write(LOWP[i]);
	}
	delay(20);

	printf("INPUT\n");
	for(int i=0;i<20;i++)
	{
		Serial.write(INPUTP[i]);
	}

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	// wait for pin to drop?

	printf("QUERY\n");
	for(int i=0;i<19;i++)
	{
		Serial.write(QUERY[i]);
	}


	b=0;
	while(Serial.available())
	{
		BUFFER[b]=Serial.read();
		b++;
	}

	for(int m=0;m<24;m++)
		printf("%x\t", BUFFER[m]);
	printf("\n");
	while (BUFFER[20] == 1) {
		delayMicroseconds(1);
		while(!Serial.available());

		b=0;
		while(Serial.available())
		{
			BUFFER[b]=Serial.read();
			b++;
		}
	}

	// read data!
	for (k = 0; k < MAXTIMINGS; k++) {
		counter = 0;
		for(int i=0;i<19;i++)
		{
			Serial.write(QUERY[i]);
		}

		while(!Serial.available());
		b=0;
		while(Serial.available())
		{
			BUFFER[b]=Serial.read();
			b++;
		}

		printf("BUFFER 20 %d\n", BUFFER[20]);
		while ( BUFFER[20] == laststate) {
			while(!Serial.available());
			b=0;
			while(Serial.available())
			{
				BUFFER[b]=Serial.read();
				b++;
			}
			counter++;
			//nanosleep(1);		// overclocking might change this?
			if (counter == 1000)
				break;
		}
		while(!Serial.available());
		b=0;
		while(Serial.available())
		{
			BUFFER[b]=Serial.read();
			b++;
		}
		laststate = BUFFER[20];
		if (counter == 1000) break;
#ifdef DEBUG
		bits[bitidx++] = counter;
#endif

		if ((k>3) && (k%2 == 0)) {
			// shove each bit into the storage bytes
			data[j/8] <<= 1;
			if (counter > 200)
				data[j/8] |= 1;
			j++;
		}
	}

#ifdef DEBUG
	for (int i=3; i<bitidx; i+=2) {
		printf("bit %d: %d\n", i-3, bits[i]);
		printf("bit %d: %d (%d)\n", i-2, bits[i+1], bits[i+1] > 200);
	}
	printf("Data (%d): 0x%x 0x%x 0x%x 0x%x 0x%x\n", j, data[0], data[1], data[2], data[3], data[4]);
#endif

	if (j >= 39) {
		checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
		if (data[4] == checksum) {
			/* yay! checksum is valid */
			/*printf("Temp = %d *C, Hum = %d \%\n", data[2], data[0]);*/
			temp_p = (float)data[2];
			hum_p = (float)data[0];

			printf("temp: %f, hum: %f\n", temp_p, hum_p);
			return 0;
		}
		return -2;
	}

	return -1;
}
