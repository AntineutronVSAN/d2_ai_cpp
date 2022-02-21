#include "random_exponential_dist.h"
#include <time.h>
#include <stdlib.h>

RandomExponentialDist::RandomExponentialDist()
{
    srand((unsigned)time(0));
}

int RandomExponentialDist::getNextInt(int maxValue, double lambda)
{

    int i;
    i = (rand() % maxValue);// + 0; 0 - С какого значения

    return i;
}
