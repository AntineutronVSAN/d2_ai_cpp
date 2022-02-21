#pragma once


const double playerLambda = 4.0;
const double aiLambda = 3.0;

class RandomExponentialDist
{
public:
	RandomExponentialDist();
	int getNextInt(int maxValue, double lambda = playerLambda);

};

