#include "arduPi.h"

void setup()
{
	Serial.begin(38400);
}

void loop()
{
	int counter =0;
	while(counter!=23)
	{
		printf("%x\t",Serial.read());
		counter++;
	}
}

int main()
{
	setup();
	while(1)
{
	loop();
	printf("\n");
}
	printf("\n");
	return 0;
}
