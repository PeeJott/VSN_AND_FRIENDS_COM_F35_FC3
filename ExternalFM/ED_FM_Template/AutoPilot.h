#pragma once

#include "State.h"
#include "Input.h"
#include "PID.h"
#include "Table.h"
#include "Vec3.h"
#include "AeroData_1.h"
#include "Maths.h"
#include "BaseComponent.h"
#include "Airframe.h"
#include "Units.h"

class AutoPilot
{
public:

	AutoPilot(State& state, Input& input, PID& pid, Airframe& airframe);

	//~AutoPilot();

	//Initialization
	virtual void zeroInit();
	virtual void coldInit();
	virtual void hotInit();
	virtual void airborneInit();

	//begin of funny functions
	//---------Update Function--------
	void AutoPilotUpdate(double dt);

	//---Sorts of AutoControllers
	void VerticalAngleControl(double dt);
	void AltitudeControl(double dt);
	void RollControl();

	//-----Return functions----------
	inline double getAutoPitch();
	inline double getAutoRoll();
			
private:

	State& m_state;
	Input& m_input;
	PID& m_pid;
	Airframe& m_airframe;

	double m_desiredAngle = 0.0;
	double m_desiredAlt = 0.0;
	double m_desiredAngleMax = 0.0;
	double m_desiredAngleMin = 0.0;

	double m_pitchUpMax = 0.0;
	double m_pitchDownMax = 0.0;

	bool m_attAPengaged = false;
	bool m_altAPengaged = false;

	double m_autoPilotPitch = 0.0;

	double m_autoPilotAngle = 0.0;

	double m_verticalControlEngaged = 0.0;



};

double AutoPilot::getAutoPitch()
{
	return m_autoPilotPitch;
}

double AutoPilot::getAutoRoll()
{

}