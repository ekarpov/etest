/*
*  Simple unit test example
*/

#include <etest.h>

int main(int argc, char* argv[])
{
    ETEST_INIT_CMD("ETest examples", argc, argv);

    ETEST_DONE();
    return ETEST_EXIT_STATUS();
}