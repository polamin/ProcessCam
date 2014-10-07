#include "time_catch.h"

time_catch::time_catch () 
{
    start = clock();
    last_duration = 0;
}

double time_catch::get_time()
{
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    return duration;
}

bool time_catch::diff_time(int second)
{
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    if (duration - last_duration > second)
    {
      last_duration = duration;
      return true;
    }
    else
      return false;
} 
