#pragma once
#include <stdio.h>
#include "Table.h"
#include "Vec3.h"
#include "State.h"
#include "Input.h"
#include "AeroData_1.h"
#include "BaseComponent.h"

//=========================================================================//
//
//		FILE NAME	: Engine.h
//		AUTHOR		: Paul Stich
//		Copyright	: Paul Stich
//		DATE		: August 2021
//
//		DESCRIPTION	: Everything concerning the engine e.g. Thrust, Heat, spooling-times, 
//					  fuel-flow and overheat-damage, warnings and indicators.
//					  
//================================ Includes ===============================//
//=========================================================================//

//#include "FlightModel.h" war doppelt in flightmodel (Ringabhängigkeit)

class Engine
{
public:
	Engine(State& state, Input& input);

	//Initialization
	virtual void zeroInit();
	virtual void coldInit();
	virtual void hotInit();
	virtual void airborneInit();

	void update(double dt); //in der () "double dt" eingefügt, war vorher ohne
	
	//----------Functions Engine 1-----------
	double FuelFlowUpdate();
	double updateThrust(); 
	double updateBurner();
	double updateSpool();
	double updateSpoolCold();
	double updateSpoolHot();

	inline double getRPMNorm();
	inline double getRPM();

	//-----------Functions Engine 2---------------
	//double FuelFlowUpdate2();
	//double updateThrust2();
	//double updateBurner2();
	//double updateSpool2();
	//double updateSpoolCold2();
	//double updateSpoolHot2();

	//inline double getRPMNorm2();
	//inline double getRPM2();
	
	//inline double getFuelFlow();//zum testen auskommentiert
	inline void setHasFuel(bool hasFuel);
	//inline void setAirspeed(double airspeed);
	inline void setIgnitors(bool ignitors);

	

	//void updateThrust(double dt);
	inline const Vec3& getForce() const;

	inline double getThrust();
	//inline double getThrust2();
	// inline void setThrust(double thrust); //auskommentiert zum Angleichen A4 engine.h

	double tempInC();
	//double tempInC2();
	
	double overHeatCount();
	//double overHeatCount2();
	double overHeat();
	//double overHeat2();
	double overHeatInd();
	//double overHeatInd2();
	//void heatCoolDown();
	
	void repairHeatDamage();
	double overSpeedInd();
	void restartNeeded();

	//void repairHeatDamage2();
	//double overSpeedInd2();
	//void restartNeeded2();



	inline void setThrottle(double throttle); //Neu eingefügt nach A4 engine2.h
	
	float DAT_EngSpool[51]{ 0.00045016, 0.00042969, 0.00041015, 0.00039150, 0.00037369, 0.00035670, 0.00034047, 0.00032499, 0.00031021, 0.00029610, 0.00028264, 0.00026978, 0.00025751, 0.00024580, 0.00023462, 0.00022395, 0.00021377, 0.00020404, 0.00019477, 0.00018591, 0.00017745, 0.00016938, 0.00016168, 0.00015433, 0.00014731, 0.00014061, 0.00013421, 0.00012811, 0.00012228, 0.00011672, 0.00011141, 0.00010635, 0.00010151, 0.00009689, 0.00009249, 0.00008828, 0.00008427, 0.00008043, 0.00007678, 0.00007328, 0.00006995, 0.00006677, 0.00006373, 0.00006083, 0.00005807, 0.00005543, 0.00005291, 0.00005050, 0.00004820, 0.00004601, 0.00004392,};
	float DAT_EngSpoolFuel[51]{ 0.00045016, 0.00042969, 0.00041015, 0.00039150, 0.00037369, 0.00035670, 0.00034047, 0.00032499, 0.00031021, 0.00029610, 0.00028264, 0.00026978, 0.00025751, 0.00024580, 0.00023462, 0.00022395, 0.00021377, 0.00020404, 0.00019477, 0.00018591, 0.00017745, 0.00016938, 0.00016168, 0.00015433, 0.00014731, 0.00014061, 0.00013421, 0.00012811, 0.00012228, 0.00011672, 0.00011141, 0.00010635, 0.00010151, 0.00009689, 0.00009249, 0.00008828, 0.00008427, 0.00008043, 0.00007678, 0.00007328, 0.00006995, 0.00006677, 0.00006373, 0.00006083, 0.00005807, 0.00005543, 0.00005291, 0.00005050, 0.00004820, 0.00004601, 0.00004392, };
	float DAT_HtoCspool[51]{ 0.0000438, 0.0000467, 0.0000498, 0.0000531, 0.0000567, 0.0000605, 0.0000645, 0.0000688, 0.0000734, 0.0000783, 0.0000835, 0.0000891, 0.0000950, 0.0001014, 0.0001081, 0.0001153, 0.0001230, 0.0001312, 0.0001400, 0.0001493, 0.0001593, 0.0001699, 0.0001813, 0.0001934, 0.0002063, 0.0002200, 0.0002347, 0.0002504, 0.0002671, 0.0002849, 0.0003039, 0.0003242, 0.0003459, 0.0003689, 0.0003936, 0.0004198, 0.0004478, 0.0004777, 0.0005096, 0.0005436, 0.0005799, 0.0006186, 0.0006598, 0.0007039, 0.0007508, 0.0008009, 0.0008544, 0.0009114, 0.0009722, 0.0010371, 0.0011063, };

	bool m_heatFailure = false;
	bool m_needRestart = false;
	bool m_needRepair = false;

	//bool m_heatFailure2 = false;
	//bool m_needRestart2 = false;
	//bool m_needRepair2 = false;

private:
	
	Vec3 m_force;
	State& m_state;
	Input& m_input;
	

	//--------------Aerodynamic Values--------------------------------
	double m_scalarVelocity = 0.0;
	double m_scalarVelocitySquared = 0.0;

	//-------------Engine Values/Commands Engine1----------------------------
	double m_thrust = 0.0;
	double m_throttle = 0.0;
	double m_burner = 0.0;
	double m_corrAirDensity = 0.0;
	
	double m_correctedFuelFlow = 0.0;
	double m_fuelFlow = 0.0; //von double zu float wegen Datenübertragung
	double m_rpmNormal = 0.0;
	double m_spoolFactor = 0.0;
	double m_spoolFactorPrevious = 0.0;
	double m_deltaSpool = 0.0;
	double m_throttleNEW = 0.0;
	double m_newThrottle = 0.0;
	double m_oldThrottle = 0.0;
	double m_deltaSpoolABS = 0.0;
	double m_newSpoolStep = 0.0;
	double m_desiredThrottle = 0.0;

	double m_spoolFactorFuel = 0.0;
	double m_deltaSpoolFuel = 0.0;
	double m_newThrottleFuel = 0.0;
	double m_oldThrottleFuel = 0.0;
	double m_deltaSpoolABSFuel = 0.0;
	double m_newSpoolStepFuel = 0.0;
	double m_spoolFactorFuel1 = 0.0;
	
	double m_spoolCStart = 0.0;
	double m_spoolCDown = 0.0;
	double m_spoolCDelta = 0.0;
	double m_spoolCDeltaABS = 0.0;
	double m_spoolCNewSpool = 0.0;
	double m_spoolCOldSpool = 0.0;
	double m_spoolCSpoolStep = 0.0;
	double m_spoolCFactor = 0.0;

	double m_spoolHStart = 0.0;
	double m_spoolHDown = 0.0;
	double m_spoolHDelta = 0.0;
	double m_spoolHDeltaABS = 0.0;
	double m_spoolHNewSpool = 0.0;
	double m_spoolHOldSpool = 0.0;
	double m_spoolHSpoolStep = 0.0;
	double m_spoolHFactor = 0.0;
	
	double m_rpmPrevious = 0.0;
	
	bool m_hasFuel = true;
	bool m_ignitors1 = true;
	
	bool m_started = false;

	double m_tempInC = 0.0;
	double m_overHeat = 0.0;
	double m_heatOne = 0.0;
	double m_heatTwo = 0.0;
	int m_heatTimerUP = 0;
	int m_heatTimerDOWN = 0;
	double m_overSpeedInd = 0.0;
	double m_overHeatInd = 0.0;

	//---------------Engine Commands and Values Engine2--------
	/*
	double m_thrust2 = 0.0;
	double m_throttle2 = 0.0;
	double m_burner2 = 0.0;
	double m_corrAirDensity2 = 0.0;

	double m_correctedFuelFlow2 = 0.0;
	double m_fuelFlow2 = 0.0; //von double zu float wegen Datenübertragung
	double m_rpmNormal2 = 0.0;
	double m_spoolFactor2 = 0.0;
	double m_spoolFactorPrevious2 = 0.0;
	double m_deltaSpool2 = 0.0;
	double m_throttleNEW2 = 0.0;
	double m_newThrottle2 = 0.0;
	double m_oldThrottle2 = 0.0;
	double m_deltaSpoolABS2 = 0.0;
	double m_newSpoolStep2 = 0.0;
	double m_desiredThrottle2 = 0.0;

	double m_spoolFactorFuel2 = 0.0;
	double m_deltaSpoolFuel2 = 0.0;
	double m_newThrottleFuel2 = 0.0;
	double m_oldThrottleFuel2 = 0.0;
	double m_deltaSpoolABSFuel2 = 0.0;
	double m_newSpoolStepFuel2 = 0.0;
	double m_spoolFactorFuel3 = 0.0;

	double m_spoolCStart2 = 0.0;
	double m_spoolCDown2 = 0.0;
	double m_spoolCDelta2 = 0.0;
	double m_spoolCDeltaABS2 = 0.0;
	double m_spoolCNewSpool2 = 0.0;
	double m_spoolCOldSpool2 = 0.0;
	double m_spoolCSpoolStep2 = 0.0;
	double m_spoolCFactor2 = 0.0;

	double m_spoolHStart2 = 0.0;
	double m_spoolHDown2 = 0.0;
	double m_spoolHDelta2 = 0.0;
	double m_spoolHDeltaABS2 = 0.0;
	double m_spoolHNewSpool2 = 0.0;
	double m_spoolHOldSpool2 = 0.0;
	double m_spoolHSpoolStep2 = 0.0;
	double m_spoolHFactor2 = 0.0;

	double m_rpmPrevious2 = 0.0;
	*/
	//die folgenden werden nur 1x gebraucht, meine ich
	/*bool m_hasFuel = true;
	bool m_ignitors = true;
	bool m_started = false;*/
	/*
	double m_tempInC2 = 0.0;
	double m_overHeat2 = 0.0;
	double m_heatOne2 = 0.0;
	double m_heatTwo2 = 0.0;
	int m_heatTimerUP2 = 0;
	int m_heatTimerDOWN2 = 0;
	double m_overSpeedInd2 = 0.0;
	double m_overHeatInd2 = 0.0;

	bool m_ignitors2 = true;
	*/

	

	//-------------Thrust Tables init------------------------
	Table PMax;
	Table PFor;
	Table CADen;
	Table EngDel;
};

const Vec3& Engine::getForce() const
{
	return m_force;
}

double Engine::getThrust()
{
	return m_thrust;
}

//void Engine::setThrust(double thrust) //rauskommentiert zur Angleichung an A4 Engine.h
//{
//	m_thrust = thrust;
//}

void Engine::setThrottle(double throttle) //neu eingefügt aus A4 engine2.h
{
	m_throttle = throttle; //die ganze Klammer neu eingefügt
}

void Engine::setHasFuel(bool hasFuel)
{
	m_hasFuel = hasFuel;
}

//zum Testen verändert
/*double Engine::getFuelFlow()
{
	m_fuelFlow = FuelFlowUpdate();
	return m_fuelFlow;
}*/

void Engine::setIgnitors(bool ignitors)
{
	m_ignitors1 = ignitors;
	//m_ignitors2 = ignitors;
}

//erstmal um eine Reaktion zu haben ist sobald ignition an und fuel da RPM auf 70%
double Engine::getRPMNorm()
{
	double RPM_Normal = 0.0;
	
	
	if ((m_hasFuel == true) && (m_ignitors1 == true) && (m_rpmPrevious < 0.685))
	{
		RPM_Normal = updateSpoolCold();
		m_rpmNormal = RPM_Normal;
		//m_rpmPrevious = m_rpmNormal;
	}

	else if ((m_hasFuel == true) && (m_ignitors1 == true)) // && (m_rpmPrevious >= 0.70))
	{
			if (updateSpool() < 0.01)
			{
				RPM_Normal = 0.70;
			}
			if (updateSpool() >= 0.01)
			{
				RPM_Normal = 0.70 + (updateSpool() * 0.40);
			}
			
			m_rpmNormal = RPM_Normal;
			//m_rpmPrevious = m_rpmNormal;
	}
	
	else if (((m_hasFuel == false) || (m_ignitors1 == false)) && (m_rpmPrevious > 0.0))
	{
		RPM_Normal = updateSpoolHot();
		m_rpmNormal = RPM_Normal;
		//m_rpmPrevious = m_rpmNormal;
	}

	else
	{
		m_rpmNormal = 0.0;
		//m_rpmPrevious = m_rpmNormal;
	}
	
	m_rpmPrevious = m_rpmNormal;

	return m_rpmNormal;
}

double Engine::getRPM()
{
	
	return m_rpmNormal * 723.0; // 100% = 7000 u/min = 7000 * 6,283 / 60 = rad/s
	//return CON_ThrToRPM * updateThrust(); //erstmal um was zu haben
}



