#inculde <stdlib.h>
#inculde "p1fxns.h"
#define UnUSed __attribute__((unused))

int main(UNUSED int argc, UNUSED char *argv[])
{
	while(p1getline(0, buf, sizeof buf))
	{
		int i = p1strchr(buf, '\n');
		buf[i] = '\0';
		i = 0;
		plputstring(1, "|");
		while((i = p1getword(buf, i word)) != -1)
		{
			p1putstr(1,word);
			p1putstr(1,"|");

		}
		p1putstr(1, "\n");
	}
	return EXIT_SUCCESS;
}