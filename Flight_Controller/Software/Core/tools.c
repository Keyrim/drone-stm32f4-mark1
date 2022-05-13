/*
 * tools.c
 *
 *  Created on: May 10
 *      Author: Théo Magne
 */

#include "tools.h"

float TOOLS_angle_pi(float angle)
{
    if(angle > PI)
    {
        angle -= 2*PI;
    }	
	else if( angle < -PI)
    {
        angle += 2*PI ;
    }
	return angle ;
}

float TOOLS_angle_diff_shorter(float to, float from)
{
	to = to - from;
	if(to > PI)
	{
		to -= 2*PI;
	}
	else if( to < -PI)
	{
		to += 2*PI ;
	}
	return to ;
}
