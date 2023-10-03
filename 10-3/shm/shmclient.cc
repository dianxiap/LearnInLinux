#include "common.hpp"

int main()
{
    key_t k = ftok(PATH_NAME, PROJ_ID);
    Log("create key done", Debug) << " client key : " << k << endl;

    
    return 0;
}