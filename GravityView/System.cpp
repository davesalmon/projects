/*
 *  System.cpp
 *  GravityComp
 *
 *  Created by David Salmon on 3/24/07.
 *  Copyright 2007 David Salmon. All rights reserved.
 *
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "System.h"
#include <cmath>
#include <stdexcept>

const double SAFETY = 0.9;
const double ERRCON = 6e-4;
const double PGROW = -0.2;
const double PSHRINK = -0.25;
const double TINY = 1.0e-30;
const double ERRORFACTOR = 1.0 / 15.0;
const double EPS = 1.0e-10;

//----------------------------------------------------------------------------------------
//  System::takeStep
//
//      take one step in the simulation.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
System::takeStep()
{
	// to take a step the following procedure is used:
    
    //	1. compute the RK params for each mass. The parameter are
    //	computed progressively, since each requires updated acc and vel
    //	for a partial step.
    //
    //	2. Take the step recording the positions of all the objects.
    //	3. Repeat 1 and 2 for two half steps.
    //	4. compute the error between 1 and 2.
    
    // use the full stop size
    computeAccelerations(location, a0);

    prevLocation = location;
    prevVelocity = velocity;
    prevTime = currentTime;
    
    double h = stepSize;
    
    // continue until we reach the specified tolerance
    while(true)
    {
        
        takeAStep(h, location, velocity, a0, s1Loc, s1Vel);
        
        h *= 0.5;
        
        double newTime = currentTime + h;
        if (newTime == currentTime)
        	throw std::domain_error("step size too small");

		newTime += h;
        // now take two steps
        takeAStep(h, location, velocity, a0, s2Loc, s2Vel);
        computeAccelerations(location, a02);
        takeAStep(h, s2Loc, s2Vel, a02, s2Loc, s2Vel);

        // compute the error
        double errMax = 0;
        for (int i = 0; i < count(); i++)
        {
            s1Loc[i].error(s2Loc[i]);
            s1Vel[i].error(s2Vel[i]);
            
            GPoint sRel(location[i]);
            sRel += velocity[i] * h;
            
        	double err = std::sqrt(s1Loc[i].hypotSq() / (sRel.hypotSq() + TINY));
            if (err > errMax)
            	errMax = err;
                
            sRel = velocity[i];
            sRel += a0[i] * h;
        	err = std::sqrt(s1Vel[i].hypotSq() / (sRel.hypotSq() + TINY));
            if (err > errMax)
            	errMax = err;
        }

		// scale to the desired precision
        errMax /= EPS;

       	h *= 2;
        // now decide how large the next step should be
        if (errMax < 1)
        {
        	stepSize = errMax > ERRCON ?
            	SAFETY * h * std::exp(PGROW*std::log(errMax)) : 4.0 * h;
            currentTime = newTime;
        	break;
        }
        
        // the error was too large. reduce the step size
        h = SAFETY * h * std::exp(PSHRINK*std::log(errMax));
    }
    
    // use the values in s1Loc and s1Vel to improve the solution
    
    for (int i = 0; i < count(); i++)
    {
    	s2Loc[i] += s1Loc[i] * ERRORFACTOR;
    	s2Vel[i] += s1Vel[i] * ERRORFACTOR;
    }

    // and store the values for the next step
    location = s2Loc;
    velocity = s2Vel;
}

//----------------------------------------------------------------------------------------
//  System::prepareSimulation
//
//      prepare the simulation by sizing each array for data.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
System::prepareSimulation()
{
	v1.resize(count());
	v2.resize(count());
	a1.resize(count());
	a2.resize(count());
	loc.resize(count());
    
    s1Loc.resize(count());
    s1Vel.resize(count());
    s2Loc.resize(count());
    s2Vel.resize(count());
    
    a0.resize(count());
    a02.resize(count());
	
	currentTime = 0;
	prevTime = 0;
    
    prepared = true;
}

//----------------------------------------------------------------------------------------
//  System::runSimulation
//
//      run the simulation for a fixed amount of time.
//
//  double stopTime                                         -> end time for sim.
//	void (*w)(void*, double t, const std::vector<GPoint>&l) -> output function.
//	void* data                                              <- data.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
System::runSimulation(double stopTime, void (*w)(void*, double t, const std::vector<GPoint>&l), void* data)
{
    if (!prepared)
    	prepareSimulation();
    
	while(currentTime < stopTime)
    {
    	takeStep();
		if (w)
			w(data, currentTime, location);
    }
}

//----------------------------------------------------------------------------------------
//  interpolate                                                                    static
//
//      interpolate fract of the way between a and b, storing result in c.
//
//  double fract   -> 
//  const std::vector<GPoint>& a  -> a values.
//	const std::vector<GPoint>& b  -> b values.
//	std::vector<GPoint>& c        <- the result at fract between a and b.
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void
interpolate(double fract, const std::vector<GPoint>& a,
			const std::vector<GPoint>& b, std::vector<GPoint>& c)
{
	for (int i = 0; i < a.size(); i++)
    {
    	c[i] = a[i] + (b[i] - a[i]) * fract;
    }
}

//----------------------------------------------------------------------------------------
//  System::getTimeAndLocation
//
//      return the time and fill result with the location.
//
//  result          <- set to the current location
//
//  returns double <- the current time
//----------------------------------------------------------------------------------------
double
System::getTimeAndLocation(std::vector<GPoint>& result)
{
	result = location;
	return currentTime;
}

// interpolate the location for t.
//----------------------------------------------------------------------------------------
//  System::interpolateLocation
//
//      interpolate the location result at time t between the current time aand the
//      previous time.
//
//  double t                     -> the time.
//  std::vector<GPoint>& result  <- the result.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
System::interpolateLocation(double t, std::vector<GPoint>& result)
{
	double currDiff = t - currentTime;
	if (currDiff > 1.0e-99)
    	throw std::domain_error("time is beyond simulation range.");
    double prevDiff = prevTime - t;
	if (prevDiff > 1.0e-99)
    	throw std::domain_error("time precedes computed range.");
    
    if (fabs(prevDiff) < 1.0e-99)
    	result = prevLocation;
    else if (fabs(currDiff) < 1.0e-99)
    	result = location;
    else
    {
    	double fract = (t - prevTime) / (currentTime - prevTime);
        interpolate(fract, prevLocation, location, result);
    }
}

//----------------------------------------------------------------------------------------
//  System::interpolateVelocity
//
//      interpolate the velocity at time t between the current time and the previous time.
//
//  double t                    -> the time.
//  std::vector<GPoint>& result <- the result.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
System::interpolateVelocity(double t, std::vector<GPoint>& result)
{
	double currDiff = t - currentTime;
	if (currDiff > 1.0e-99)
    	throw std::domain_error("time is beyond simulation range.");
    double prevDiff = prevTime - t;
	if (prevDiff > 1.0e-99)
    	throw std::domain_error("time precedes computed range.");
    
    if (fabs(prevDiff) < 1.0e-99)
    	result = prevVelocity;
    else if (fabs(currDiff) < 1.0e-99)
    	result = velocity;
    else
    {
    	double fract = (t - prevTime) / (currentTime - prevTime);
        interpolate(fract, prevVelocity, velocity, result);
    }
}

//----------------------------------------------------------------------------------------
//  System::printVector
//
//      print the message followed by the points in the vector.
//
//  const char* msg              -> the message.
//  const std::vector<GPoint>& v -> the vector.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
System::printVector(const char* msg, const std::vector<GPoint>& v)
{
	printf("%s", msg);
    for (int i = 0; i < v.size(); i++)
    	printf("(%8.4g, %8.4g), ", v[i].getX(), v[i].getY());
    printf("\n\n");
}

//----------------------------------------------------------------------------------------
//  stepDerivatives                                                                static
//
//      compute the values for y step time units in the future, given the derivatives.
//
//  double step                     -> the step to take.
//  const std::vector<GPoint>& dydt -> the derivates of f.
//
//	std::vector<GPoint>& y          <- the new values for f.
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void
stepDerivatives(double step, const std::vector<GPoint>& dydt, std::vector<GPoint>& y)
{
    for (int i = 0; i < dydt.size(); i++)
    {
    	y[i] += (dydt[i] * step);
	}
}

// compute the accelerations for all the objects in the system.
//----------------------------------------------------------------------------------------
//  System::computeAccelerations
//
//      compute the accelerations for each object.
//
//		locs are the current locations for each object
//		acceleration is the acceleration for each object.
//
//		F = G m1 m2/r^2
//		a1 = F/m1 = (G m2)/r^2
//		each mass in the system is (G m)
//		diff/r is a unit vector in the a direction.
//
//  const std::vector<GPoint>& locs   -> locations
//	std::vector<GPoint>& acceleration <- accelerations
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
System::computeAccelerations(const std::vector<GPoint>& locs, std::vector<GPoint>& acceleration) const
{
	for (int i = 0; i < locs.size(); i++)
    {
        GPoint& currAcc = acceleration[i];
		// initialize to 0
		currAcc = GPoint();
        //double mass = masses[i];
        
        for (int j = 0; j < locs.size(); j++)
        {
        	if (i != j)
            {
            	GPoint diff = locs[j] - locs[i];
                double magSq = diff.hypotSq();
                double factor = magSq * std::sqrt(magSq);
                double accVal = masses[j] / factor;
                
                currAcc += (diff * accVal);	// total acc
            }
        }
    }
}

//----------------------------------------------------------------------------------------
//  System::takeAStep
//
//      take one step starting with locations, x0, speed v0, and acceleration, accl.
//      return the new position and velocity in outPos and outVel, respectively.
//
// Basically, the equations we want to solve are
//	r'' = F(t, r)/m	where F = gravitation force, and m is the mass
//
//	These are written as
//	r' = v	 = fr(t,r)
//	v' = F/m = fv(t,r,v)
//
// The RK- params are
//	kr0 = fr(t0,     r0,           v0)			 = v(t0)
//	kr1 = fr(t0+h/2, r0 + h kr0/2, v0 + h kv0/2) = v(t0+h/2)
//	kr2 = fr(t0+h/2, r0 + h kr1/2, v0 + h kv1/2) = v(t0+h/2)
//	kr3 = fr(t0+h,   r0 + h kr3  , v0 + h kv3)	 = v(t0+h)
//
//	kv0 = fv(t0,     r0,           v0)			 = F(t0)/m
//	kv1 = fv(t0+h/2, r0 + h kr0/2, v0 + h kv0/2) = F(t0+h/2)/m
//	kv2 = fv(t0+h/2, r0 + h kr1/2, v0 + h kv1/2) = F(t0+h/2)/m
//	kv3 = fv(t0+h,   r0 + h kr3  , v0 + h kv3)	 = F(t0+h)/m
//
//  double step                     -> the step to take
//  const std::vector<GPoint>& x0   -> the locations at the start of step.
//	const std::vector<GPoint>& v0   -> the velocities at the start of step.
//	const std::vector<GPoint>& acc1 -> the accelerations at the start of step.
//	std::vector<GPoint>&outPos      <- the new locations.
//	std::vector<GPoint>&outVel      <- the new velocities.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
System::takeAStep(double step, 
		const std::vector<GPoint>& x0,
        const std::vector<GPoint>& v0,
		const std::vector<GPoint>& acc1, 
        std::vector<GPoint>&outPos, std::vector<GPoint>&outVel)
{
 	// for the first, step, the derivatives are in acceleration and velocity
    loc = x0;
    v1 = v0;
    
    // update the location, from the step and velocity for kr1
    stepDerivatives(0.5 * step, velocity, loc);
    
    // update the velocities from the accelerations for kv1.
    stepDerivatives(0.5 * step, acc1, v1);
    
    // compute the derivates of v for vk2
    computeAccelerations(loc, a1);
    v2 = v0;
	loc = x0;
	
	// use the rk1 and rv1 values to get rk2, rv2
    stepDerivatives(0.5 * step, v1, loc);
    stepDerivatives(0.5 * step, a1, v2);

	// third step
    computeAccelerations(loc, a2);
    loc = x0;

    stepDerivatives(step, v2, loc);
    
    // sum the derivatives into v2 and a2 so we can reuse v1 and a1
    for (int i = 0; i < v2.size(); i++)
    {
    	v2[i] += v1[i];
        v2[i] *= 2.0;
    }
    
    v1 = v0;
    stepDerivatives(step, a2, v1);
	
        // sum the derivatives into v2 and a2 so we can reuse v1 and a1
    for (int i = 0; i < v2.size(); i++)
    {
    	a2[i] += a1[i];
        a2[i] *= 2.0;
    }
    
    // and the fourth stes
    computeAccelerations(loc, a1);

	double fact = step/6.0;
	// now compute the result
    for (int i = 0; i < outPos.size(); i++)
    {
    	outPos[i] = x0[i] + (v0[i] + v1[i] + v2[i]) * fact;
        outVel[i] = v0[i] + (acc1[i] + a1[i] + a2[i]) * fact; 
    }
}
