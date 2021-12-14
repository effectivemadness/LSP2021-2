#include <stdio.h>

void del(char str[])
{
	int i = 0;
	while (str[i] != '\0')
	{
		str[i] = str[i+1];
		i++;
	}
}

int main()
{
	char str[20] = "Hello";
	del(str);
	puts(str);
}
