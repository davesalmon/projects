/*
 *  System.h
 *  GravityComp
 *
 *  Created by David Salmon on 3/24/07.
 *  Copyright 2007 David Salmon. All rights reserved.
 *
 */
/*
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

#include <vector>
#include "GPoint.h"

const int STEP_FACTOR = 32;

//const double GG = 8.89449e-10;	// AU^3 / (Day^2 EM) where EM = earth mass

class System
{
public:
	System() : stepSize(1.0/STEP_FACTOR), prevTime(0), currentTime(0), prepared(false) {}
    
    // add a new mass to the system.
	void addMass(const GPoint& loc, const GPoint& vel, const double mg) 
    {
        location.push_back(loc);
        velocity.push_back(vel);
        masses.push_back(mg);
        prevLocation.push_back(loc);
        prevVelocity.push_back(loc);
        
        // and reset
        prepared = false;
    }
        
    // return the number of masses defined.
    int	count() const { return masses.size(); }
        
    // return the mass list.
    const std::vector<double>& getMasses() const { return masses; }
    
    // print out the specified vector with
    //	the message. This is for debugging.
    void printVector(const char* msg, const std::vector<GPoint>& v);
            
    // run the simulation for the specified time increment
    void runSimulation(double stopTime, void (*w)(void*, double t, const std::vector<GPoint>&l), void* data);
    
    // create vector of the requisite size
    std::vector<GPoint> makeResultVector() const 
    {
    	std::vector<GPoint> v;
        v.resize(count());
        return v;
    }
    
    // interpolate the location/velocity at t. t must lie between prevTime
    //	and current time. typically the value for stopTime in 
    //	run simulation will be used.
    void interpolateLocation(double t, std::vector<GPoint>& result);
    void interpolateVelocity(double t, std::vector<GPoint>& result);
	
	// return the time and the current location for that time.
	double getTimeAndLocation(std::vector<GPoint>& result);
    
private:

    void takeStep();

    void prepareSimulation();

	// interpolate the location for t putting the results into location.
	//void getLocationForTime(double t, std::vector<GPoint>& location);

	// take a step using the specified step and masses and update masses.
	void takeAStep(double step, 
    		const std::vector<GPoint>& x0, 
            const std::vector<GPoint>& v0,
    		const std::vector<GPoint>& acc1,
    		std::vector<GPoint>& outPos,
            std::vector<GPoint>& outVel);
	void computeAccelerations(const std::vector<GPoint>& locs, 
    		std::vector<GPoint>& acceleration) const;


	std::vector<GPoint> 	location;			// the locations for each mass
    std::vector<GPoint> 	velocity;			// the velocities for each mass
    std::vector<double>	masses;				// the mass of each
    
    std::vector<GPoint> prevLocation;	// the previous location for interp.
    std::vector<GPoint> prevVelocity;	// the previous velocity for interp.

    double stepSize;					// the current adaptive step size
    double prevTime;					// the time for the previous
    double currentTime;					// the time for the current
    
    // these are used for intermediate derivatives
    bool prepared;
    std::vector<GPoint>	v1;
    std::vector<GPoint>	v2;
    std::vector<GPoint>	a1;
    std::vector<GPoint>	a2;
    std::vector<GPoint>	loc;
    
    std::vector<GPoint>	a0;
    std::vector<GPoint>	a02;
    
    std::vector<GPoint>	s1Loc;
    std::vector<GPoint>	s1Vel;
    
    std::vector<GPoint>	s2Loc;
    std::vector<GPoint>	s2Vel;
    
};