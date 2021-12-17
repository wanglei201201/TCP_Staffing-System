#include "ser.h"

void handler(int sig)                                                                                                   
{
	do_exit();
	exit(0);
    return ;
}

int main(int argc, const char *argv[])
{
	sighandler_t s = signal(2, handler);
	if(SIG_ERR == s)
	{
		perror("signal");
		return -1;
	}
	cli_init();
	return 0;
}


