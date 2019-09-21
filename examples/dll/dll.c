#include <stdio.h>

#ifdef _MSC_VER
// On visual studio, we need to use this "decoration" on our function
#define EXPORT __declspec(dllexport)
#else
// On other platform, we just define this symbol so that the code will compile
#define EXPORT
#endif

void EXPORT print_hello()
{
	printf("Hello DLL!\n");
}

int EXPORT get_the_answer_to_life_the_universe_and_everything()
{
	return 42;
}
