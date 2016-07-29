#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
long tdel;

if (argc < 2)
	{
	printf("calling format is msleep <delay> where delay is milliseconds\n");
	exit(0);
	}
	
sscanf(argv[1], "%ld", &tdel);
tdel *= 1000L;
/* printf("sleeping for %ld microseconds\n",tdel); */

usleep(tdel);

}
