#include <iostream>
#include <ctime>
using namespace std;

class time_catch {

  public:
    time_catch();
    double  get_time(); 
    bool    diff_time(int);

  private: 
    clock_t   start;
    double    duration;
    double    last_duration;

};
