#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	int i = 1;
	const int ic = i;
	const int *pic = &ic;
	int *const cpi=&i;

	return 0;
}
