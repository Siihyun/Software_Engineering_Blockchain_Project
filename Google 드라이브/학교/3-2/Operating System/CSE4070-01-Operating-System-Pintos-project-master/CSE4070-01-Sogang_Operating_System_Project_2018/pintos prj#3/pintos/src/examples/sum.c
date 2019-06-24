#include <syscall.h>
#include <stdio.h>

int
main (int argc, char* argv[])
{
	int sol1,sol2;
	sol1 = pibonacci(atoi(argv[1]));
	sol2 = sum_of_four_integers(atoi(argv[1]) , atoi(argv[2]), atoi(argv[3]) , atoi(argv[4]));
	printf("%d %d\n",sol1,sol2);
	return EXIT_SUCCESS;
}








