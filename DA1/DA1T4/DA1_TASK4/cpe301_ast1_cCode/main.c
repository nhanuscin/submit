/*
 * cpe301_ast1_cCode.c
 *
 * Created: 2/17/2018 8:04:02 PM
 * Author : Nathan Hanuscin
 */ 

#include <avr/io.h>
#include <stdio.h>

int main()
{
	int sum5 = 0;
	int sumn5 = 0;
	int num = 1;
	
	for (int i=0; i<300; i++)
		{
			if(num%5 ==0)
				sum5 = sum5 + num;
			else
				sumn5 = sumn5 + num;
			num++;
			if(num > 255)
				num = 0;
		}
printf ("Divisible by 5 sum is %i\n", sum5);
printf ("Not divisible by 5 sum is %i\n", sumn5);
return 0;
}

