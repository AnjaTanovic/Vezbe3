//Napraviti aplikaciju za rad sa drajverima za LED i Tastere: 
//Pri cemu se aplikacija vrti u beskonacnoj petlji i pravi PWM signal. 
//Inicijalno su sve LED iskljucene. Sa svakim pritiskom tastera 1 se 
//faktor ispune poveca za 10%, a svakim pritiskom tastera 0 se faktor 
//ispune smanji za 10%. Pripaziti na softversko diferenciranje i na 
//limite da faktor ispune ne moze biti veci od 100% niti manji od 0%. 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	FILE *fp;
	int flag = 0; //za softversko diferenciranje

	//tasteri
	char *str;
	char *korak;
	float x = 0.1;
	char tval1,tval2,tval3,tval4;
	char tval1_x, tval2_x; //pomocne za softv diferenciranje
	size_t num_of_bytes = 6;

	//led
	float percentage = 0; //povecava-t2 se ili smanjuje-t1 za 0.1 (10%)
	long int period = 20000L;

	fp = fopen("/dev/led", "w");
	if(fp == NULL)
	{
		printf("Problem pri otvaranju /dev/led\n");
		return -1;
	}
	fputs("0x00\n", fp); //inicjalno iskljucene

	while(1)
	{
		//Citanje vrednosti prekidaci
		fp = fopen ("/dev/switch", "r");
		if(fp==NULL)
		{
			puts("Problem pri otvaranju /dev/switch");
			return -1;
		}
		korak = (char *)malloc(num_of_bytes+1); 
		getline(&korak, &num_of_bytes, fp);
		
		if(fclose(fp))
		{
			puts("Problem pri zatvaranju /dev/button");
			return -1;
		}

		if (korak[4] == '0')
		{
			if (korak[5] == '0')	
				x = 0.05;
			else
				x = 0.1;
		}
		else
		{
			if (korak[5] == '0')	
				x = 0.15;
			else
				x = 0.2;
		}
		
		//Citanje vrednosti tastera
		fp = fopen ("/dev/button", "r");
		if(fp==NULL)
		{
			puts("Problem pri otvaranju /dev/button");
			return -1;
		}
		str = (char *)malloc(num_of_bytes+1); 
		getline(&str, &num_of_bytes, fp);

		if(fclose(fp))
		{
			puts("Problem pri zatvaranju /dev/button");
			return -1;
		}


		tval1 = str[2] - 48;
		tval2 = str[3] - 48;
		tval3 = str[4] - 48;
		tval4 = str[5] - 48;
		free(str);

		if (tval1_x != tval1 || tval2_x != tval2)
		{
			flag = 1;
			tval1_x = tval1;
			tval2_x = tval2;
		}
		else
			flag = 0;

		//trebaju samo tasteri tval1 i tval2
		if (flag == 1)
		{
			if (tval1 == 1)
			{
				percentage += x;
				if ( percentage > 1)
					percentage = 1;
			}
			else if (tval2 == 1)
			{
				percentage -= x;
				if (percentage < 0)
					percentage = 0;
			}
		}

		//realizacija izabranog faktora ispune
		fp = fopen("/dev/led", "w");
		if(fp == NULL)
		{
			printf("Problem pri otvaranju /dev/led\n");
			return -1;
		}
		fputs("0x0F\n", fp);
		if(fclose(fp))
		{
			printf("Problem pri zatvaranju /dev/led\n");
			return -1;
		}
		usleep(percentage*period);

		fp = fopen("/dev/led", "w");
		if(fp == NULL)
		{
			printf("Problem pri otvaranju /dev/led\n");
			return -1;
		}
		fputs("0x00\n", fp);
		if(fclose(fp))
		{
			printf("Problem pri zatvaranju /dev/led\n");
			return -1;
		}
		usleep((1-percentage)*period);
	}

	return 0;
}
