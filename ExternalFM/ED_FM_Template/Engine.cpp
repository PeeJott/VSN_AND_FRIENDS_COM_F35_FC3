#include "Engine.h"

//=========================================================================//
//
//		FILE NAME	: Engine.cpp
//		AUTHOR		: Paul Stich
//		Copyright	: Paul Stich
//		DATE		: August 2021
//
//		DESCRIPTION	: Everything concerning the engine e.g. Thrust, Heat, spooling-times, 
//					  fuel-flow and overheat-damage, warnings and indicators.
//					  
//================================ Includes ===============================//
//=========================================================================//

Engine::Engine
(
	State& state,
	Input& input

) :
	m_state(state),
	m_input(input),
	//---------------Thrust------------------------------------
	PMax(DAT_PMax, CON_PMaxmin, CON_PMaxmax),
	PFor(DAT_PFor, CON_PFormin, CON_PFormax),
	CADen(DAT_CADen, CON_CADenMin, CON_CADenMax),
	EngDel(DAT_EngDel, CON_EngDelMin, CON_EngDelMax)
	//der letzte Eintrag darf KEIN Komma haben...
{
	//huhu!!
}

void Engine::zeroInit()
{
	m_scalarVelocity = 0.0;
	m_scalarVelocitySquared = 0.0;
	//-------------Engine Values/Commands----------------------------
	m_thrust = 0.0;
	
	m_burner = 0.0;
	m_fuelFlow = 0.0;
	m_correctedFuelFlow = 0;
	m_corrAirDensity = 0.0;
	
	m_spoolFactor = 0.0;
	m_spoolFactorPrevious = 0.0;
	m_deltaSpool = 0.0;
	m_throttleNEW = 0.0;
	m_newThrottle = 0.0;
	m_oldThrottle = 0.0;
	m_deltaSpoolABS = 0.0;
	m_newSpoolStep = 0.0;
	m_desiredThrottle = 0.0;

	m_spoolFactorFuel = 0.0;
	m_deltaSpoolFuel = 0.0;
	m_newThrottleFuel = 0.0;
	m_oldThrottleFuel = 0.0;
	m_deltaSpoolABSFuel = 0.0;
	m_newSpoolStepFuel = 0.0;
	m_spoolFactorFuel1 = 0.0;

	m_needRestart = false;
	m_needRepair = false;

	m_spoolCDelta = 0.0;
	m_spoolCDeltaABS = 0.0;
	m_spoolCDown = 0.0;
	m_spoolCNewSpool = 0.0;
	m_spoolCOldSpool = 0.0;
	m_spoolCSpoolStep = 0.0;
	m_spoolCStart = 0.0;
	m_spoolCFactor = 0.0;

	m_spoolHDelta = 0.0;
	m_spoolHDeltaABS = 0.0;
	m_spoolHFactor = 0.0;
	m_spoolHNewSpool = 0.0;
	m_spoolHOldSpool = 0.70;
	m_spoolHSpoolStep = 0.0;

	m_rpmNormal = 0.0;
	m_rpmPrevious = 0.0;

	m_tempInC = 0.0;

	m_overHeat = 0.0;
	m_heatOne = 0.0;
	m_heatTwo = 0.0;
	m_heatTimerUP = 0;
	m_heatTimerDOWN = 0;
	m_overSpeedInd = 0.0;
	m_heatFailure = false;
	m_overHeatInd = 0.0;
	
	
	m_hasFuel = true;
	m_ignitors1 = false;
	
	//m_input.m_engine_start = 0.0; auskomentiert da in Funktionen
	//m_input.m_engine_stop = 0.0;
	

}

void Engine::coldInit()
{
	zeroInit();
	
}

void Engine::hotInit()
{
	zeroInit();

	m_ignitors1 = true;
	
	m_rpmNormal = 0.72;
	m_rpmPrevious = 0.72;
	//m_input.m_engine_start = 1.0;
	m_spoolHOldSpool = 0.70;
	m_spoolCOldSpool = 0.71;

	
	//m_input.m_engine_start = 1.0;
	
}

void Engine::airborneInit()
{
	zeroInit();

	m_ignitors1 = true;
	//m_ignitors2 = true;
	m_rpmNormal = 0.85;
	m_rpmPrevious = 0.85;
	//m_input.m_engine_start = 1.0;
	m_spoolHOldSpool = 0.70;
	m_spoolCOldSpool = 0.71;

	//m_rpmNormal2 = 0.85;
	//m_rpmPrevious2 = 0.85;
	//m_input.m_engine_start = 1.0;
	//m_spoolHOldSpool2 = 0.70;
	//m_spoolCOldSpool2 = 0.71;
}

void Engine::update(double dt)
{
	double corrThrottle = 0.0;
	
	if ((m_input.getEngineStart1() == 1.0) && (m_needRepair == false) && (m_heatFailure == false) && (m_needRestart == false))
	{
		m_ignitors1 = true;
	}
	if ((m_input.getEngineStop1() == 1.0) || (m_input.getEngineStart1() == 0.0) || (m_needRepair == true) || (m_heatFailure == true) || (m_needRestart == true))
	{
		m_ignitors1 = false;
		//m_input.m_engine_start = 0.0;
		//m_input.m_engine_stop = 0.0;
	}

	


	//------------NEW corrected Air Density with a lot of steps through lookup----------
	//-------------OLD Version---------------------------------------------------------
	/*if (m_state.m_airDensity <= 0.88)
	{
		m_corrAirDensity = CADen(m_state.m_airDensity);
	}
	else
	{
		m_corrAirDensity = 0.0;
	}*/
	//-----------------NEW Version mit CAD bis SL---------------------------------------
	m_corrAirDensity = CADen(m_state.m_airDensity);

//--------------END of corrected airdensity----------------------------------------------
//--------------Start of Engine starting up and spooling down//Noch dr�ber nachdenken----------------------------	
	
	//RESET updateSpoolCold Engine 1
	if ((m_ignitors1 == false) || (m_hasFuel == false) || (overHeat() == 1.0))
	{
		m_spoolCOldSpool = getRPMNorm();
		m_spoolCDelta = 0.0;
		m_spoolCDeltaABS = 0.0;
		m_spoolCFactor = 0.0;
		m_spoolCNewSpool = 0.0;
		m_spoolCSpoolStep = 0.0;

	}

	//RESET updateSpoolHot Engine 1
	if ((m_ignitors1 == true) && (m_hasFuel == true))
	{
		m_spoolHOldSpool = getRPMNorm();
		m_spoolHDelta = 0.0;
		m_spoolHDeltaABS = 0.0;
		m_spoolHFactor = 0.0;
		m_spoolHNewSpool = 0.0;
		m_spoolHSpoolStep = 0.0;

	}

	

	

	//heatCoolDown();

	//printf("Throttle_Position %f \n", m_input.getThrottle());
	/*printf("FuelIndicator %f \n", m_heatTimerUP);
	printf("OHStage %f \n", m_overHeat);
	printf("OverSpeedInd %f \n", m_overSpeedInd);
	printf("Mach %f \n", m_state.m_mach);
	printf("AirDensity %f \n", m_state.m_airDensity);*/

	//-------------NEU eingef�gt wegen Replay-K�se--------------
	updateThrust();
	FuelFlowUpdate();
	updateSpool();
	updateBurner();
	updateSpoolCold();
	updateSpoolHot();
	tempInC();
	overHeat();
	overHeatCount();
	overSpeedInd();
	restartNeeded();
	overHeatInd();

	
}

double Engine::updateThrust() //Wenn Ver�nderungen dann hier ver�ndern NICHT oben!!!!! //dt in die Klammer eingef�gt//double zu void mit double dt ver�ndert
{
	m_force = Vec3(); //braucht man hier vielleicht nicht, trotzdem wieder eingesetzt zum testen
	m_throttle = 0.0; //neu eingef�gt
	m_thrust = 0.0; //wieder eingef�gt nach auskommentierung
	//m_thrust = 150000; //nur zum testen, wieder rausnehmen
	
	m_scalarVelocity = magnitude(m_state.m_localAirspeed);
	m_scalarVelocitySquared = m_scalarVelocity * m_scalarVelocity;
	m_state.m_mach = m_scalarVelocity / m_state.m_speedOfSound;
	
	//corrThrottle = -0.55 * m_input.m_throttle + 1.0; //TESTSETTINGS
	//m_thrust = corrThrottle * PFor(m_state.m_mach); //TESTSETTINGS
	
	//printf("Throttle %f \n", corrThrottle);
	//printf("Thrust %f \n", m_thrust); //eingef�gt zum testen, ob M_thrust ausgegeben wird...
	//printf("CurrentAirDensity %f \n", m_state.m_airDensity);
	bool nozzleIsStraight = true;

	if (m_input.getTiltEngineNozzle() >= 0.2)
	{
		nozzleIsStraight = false;
	}
	else
	{
		nozzleIsStraight = true;
	}

	//-----------Thrust-function---------------------------------------------------------------------------
	//bei PMax den airdensity-correction-multiplyer eingef�gt. Dieser bedarf einer �berarbeitung zusammen mit dem Drag und Thrust-profil...
	//
	
	if ((updateSpool() <= 0.85) && (m_ignitors1 == true) && (m_hasFuel == true) && (getRPMNorm() >= 0.70))
	{
		m_thrust = (updateSpool() * PMax(m_state.m_mach)) * ((m_state.m_airDensity + ( 0.66 * m_corrAirDensity)) / CON_sDay_den);
	}
	else if ((updateSpool() > 0.85) && (m_ignitors1 == true) && (m_hasFuel == true) && (getRPMNorm() >= 0.70) && (nozzleIsStraight == false))
	{
		m_thrust = (updateSpool() * PMax(m_state.m_mach)) * ((m_state.m_airDensity + (0.66 * m_corrAirDensity)) / CON_sDay_den);
	}
	else if ((updateSpool() > 0.85) && (m_ignitors1 == true) && (m_hasFuel == true) && (getRPMNorm() >= 0.70) && (nozzleIsStraight == true))
	{
		m_thrust = (updateSpool() * PFor(m_state.m_mach)) * ((m_state.m_airDensity + m_corrAirDensity) / CON_sDay_den);
	}
	else
	{
		m_thrust = 0.0;
	}
	
	return m_thrust;
}

double Engine::FuelFlowUpdate() 
{
	bool nozzleIsStraight = true;

	if (m_input.getTiltEngineNozzle() >= 0.2)
	{
		nozzleIsStraight = false;
	}
	else
	{
		nozzleIsStraight = true;
	}
	
	double lowOmegaInertia = 1.0;

	double spoolUpFactor = 0.0;

	if ((getRPMNorm() < 0.70) && (m_ignitors1 == true) && (m_hasFuel == true))
	{
		spoolUpFactor = updateSpoolCold();
	}
	else
	{
		spoolUpFactor = 1.0;
	}
	
	
	double corrThrottle = 0.0;

		
	if (m_input.getThrottle() >= 0.0)
	{
		corrThrottle = (1.0 - CON_ThrotIDL) * m_input.getThrottle() + CON_ThrotIDL;
	}
	else
	{
		corrThrottle = (m_input.getThrottle() + 1.0) / 2.0;
	}
	

	
	m_deltaSpoolFuel = corrThrottle - m_oldThrottleFuel;
	m_deltaSpoolABSFuel = abs(m_deltaSpoolFuel);
	//m_spoolFactor = EngDel(m_deltaSpoolABS);//die alte Idee mit der Alten "Table"
	int indexInArray = m_deltaSpoolABSFuel / 0.02; //hier wird der Index f�r den Array aus einer "double" in eine "int" umgem�nzt, damit ganze Zahlen generiert werden// kein Factor, damit nicht au�erhalb des Arrays verschoben wird
	m_spoolFactorFuel = (DAT_EngSpoolFuel[indexInArray]);//Variale indexInArray ergibt diejenige Zahl die dem Index in dem Array entspricht// /5 hinzugef�gt, f�r langsameres Ansprechen testweise
	m_spoolFactorFuel1 = m_spoolFactorFuel * 10;
	m_newSpoolStepFuel = m_deltaSpoolFuel * m_spoolFactorFuel1;
	m_newThrottleFuel = m_oldThrottleFuel + m_newSpoolStepFuel;

	m_oldThrottleFuel = m_newThrottleFuel; //m_newThrottleFuel ist der Wert anstatt des corrThrottle

	//printf("Throttle %f \n", corrThrottle);
	//printf("NEW_Throttle %f \n", m_newThrottle);
	
	
	//-------------------Begin FuelFlow-NEW angelehnt an Leistung kN-----------------------------------
	
	if ((corrThrottle < 0.01) && (m_ignitors1 == true) && (m_hasFuel == true))
	{
		m_fuelFlow = spoolUpFactor * (1500.0 * (m_state.m_airDensity / CON_sDay_den) + (m_state.m_mach * (0.60 * m_newThrottleFuel * (CON_FuCoMil * PMax(m_state.m_mach) / 1000.0))));
	}

	else if ((corrThrottle >= 0.01) && (corrThrottle < 0.85) && (m_ignitors1 == true) && (m_hasFuel == true))
	{
		m_fuelFlow = spoolUpFactor *((m_newThrottleFuel * ((CON_FuCoMil * PMax(m_state.m_mach)) / 1000.0) + 1500.0) * (m_state.m_airDensity / CON_sDay_den)); //+ (m_state.m_mach * (0.60 * updateSpool() * CON_CeMax * 3600 * 2.205));
	}
	else if ((corrThrottle >= 0.85) && (m_ignitors1 == true) && (m_hasFuel == true) && (nozzleIsStraight == true))
	{
		m_fuelFlow = spoolUpFactor * ((m_newThrottleFuel * ((CON_FuCoAB * PFor(m_state.m_mach)) / 1000.0) + 1500.0) * (m_state.m_airDensity / CON_sDay_den)); // (m_state.m_mach * (0.60 * updateSpool() * CON_CeFor * 3600 * 2.205));
	}
	else
	{
	m_fuelFlow = 0.0;
	}

	return m_fuelFlow;
}

double Engine::updateSpool()
{
	double corrThrottle = 0.0;


	if (m_input.getThrottle() >= 0.0)
	{
		corrThrottle = (1.0 - CON_ThrotIDL) * m_input.getThrottle() + CON_ThrotIDL;
	}
	else
	{
		corrThrottle = (m_input.getThrottle() + 1.0) / 2.0;
	}
	
	
	
	m_deltaSpool = corrThrottle - m_oldThrottle;
	m_deltaSpoolABS = abs(m_deltaSpool);
	//m_spoolFactor = EngDel(m_deltaSpoolABS);//die alte Idee mit der Alten "Table"
	int indexInArray = m_deltaSpoolABS / 0.02; //hier wird der Index f�r den Array aus einer "double" in eine "int" umgem�nzt, damit ganze Zahlen generiert werden// kein Factor, damit nicht au�erhalb des Arrays verschoben wird
	m_spoolFactor = (DAT_EngSpool[indexInArray]) / 6.0;//Variale indexInArray ergibt diejenige Zahl die dem Index in dem Array entspricht// /5 hinzugef�gt, f�r langsameres Ansprechen testweise/ jetzt auf 6.0 erh�ht
	m_newSpoolStep = m_deltaSpool * m_spoolFactor;
	m_newThrottle = m_oldThrottle + m_newSpoolStep;
	
	m_oldThrottle = m_newThrottle;

	//printf("Throttle %f \n", corrThrottle);
	//printf("NEW_Throttle %f \n", m_newThrottle);
	return m_newThrottle;

	//double spool[50] = { 1.0000, 0.7817, 0.6753, 0.5834, 0.5040, 0.4354, 0.3762, 0.3250, 0.2808, 0.2425, 0.2095, 0.1810, 0.1564, 0.1351, 0.1167, 0.1008, 0.0871, 0.0753, 0.0650, 0.0562, 0.0485, 0.0419, 0.0362, 0.0313, 0.0270, 0.0234, 0.0202, 0.0174, 0.0151, 0.0130, 0.0112, 0.0097, 0.0084, 0.0072, 0.0063, 0.0054, 0.0047, 0.0040, 0.0035, 0.0030, 0.0026, 0.0022, 0.0019, 0.0017, 0.0014, 0.0013, 0.0011, 0.0009, 0.0008, 0.0007,};// voll aufgef�llt
}

double Engine::updateSpoolCold()
{
	double spoolColdEnd = 0.71;
	

		m_spoolCDelta = spoolColdEnd - m_spoolCOldSpool;
		m_spoolCDeltaABS = abs(m_spoolCDelta);
		//m_spoolFactor = EngDel(m_deltaSpoolABS);//die alte Idee mit der Alten "Table"
		int indexInArray = m_spoolCDeltaABS / 0.014; //hier wird der Index f�r den Array aus einer "double" in eine "int" umgem�nzt, damit ganze Zahlen generiert werden// kein Factor, damit nicht au�erhalb des Arrays verschoben wird
		m_spoolCFactor = (DAT_EngSpool[indexInArray]) / 22;//von /10 auf / 15 hochgesetzt f�r langsameren Cold-Start//Variable indexInArray ergibt diejenige Zahl die dem Index in dem Array entspricht
		m_spoolCSpoolStep = m_spoolCDelta * m_spoolCFactor;
		m_spoolCNewSpool = m_spoolCOldSpool + m_spoolCSpoolStep + 0.000001;
		m_spoolCOldSpool = m_spoolCNewSpool;

		//printf("Throttle %f \n", corrThrottle);
		//printf("NEW_Throttle %f \n", m_newThrottle);
		return m_spoolCNewSpool;
}

double Engine::updateSpoolHot()
{
	double SpoolHotEnd = -0.1;



	m_spoolHDelta = SpoolHotEnd - m_spoolHOldSpool;
	m_spoolHDeltaABS = abs(m_spoolHDelta);
	//m_spoolFactor = EngDel(m_deltaSpoolABS);//die alte Idee mit der Alten "Table"
	int indexInArray = m_spoolHDeltaABS / 0.022; //hier wird der Index f�r den Array aus einer "double" in eine "int" umgem�nzt, damit ganze Zahlen generiert werden// kein Factor, damit nicht au�erhalb des Arrays verschoben wird
	m_spoolHFactor = (DAT_HtoCspool[indexInArray]) / 120.0;//Variale indexInArray ergibt diejenige Zahl die dem Index in dem Array entspricht
	m_spoolHSpoolStep = (m_spoolHDelta * m_spoolHFactor) - 0.00001;
	m_spoolHNewSpool = m_spoolHOldSpool + m_spoolHSpoolStep;
	m_spoolHOldSpool = m_spoolHNewSpool;

	//printf("Throttle %f \n", corrThrottle);
	//printf("NEW_Throttle %f \n", m_newThrottle);


	return m_spoolHNewSpool;
}

double Engine::updateBurner()
{
	m_burner = 0.0;

	bool nozzleIsStraight = true;

	if (m_input.getTiltEngineNozzle() >= 0.2)
	{
		nozzleIsStraight = false;
	}
	else
	{
		nozzleIsStraight = true;
	}
	//------------------Alte Burner-Funktion die funktioniert----------------
	/*double corrThrottle = 0.0;

	if (m_input.m_throttle >= 0.0)
	{
		corrThrottle = ((1 - CON_ThrotIDL) * m_input.m_throttle + CON_ThrotIDL);
	}
	else
	{
		corrThrottle = ((m_input.m_throttle + 1.0) / 2.0);
	}*/

	if ((updateSpool() >= 0.85) && (m_ignitors1 == true) && (m_hasFuel == true) && (nozzleIsStraight == true))
	{
		m_burner = 1.0;
	}
	else
	{
		m_burner = 0.0;
	}
	return m_burner;
}

double Engine::tempInC()
{
	m_tempInC = 0.0;

	if (getRPMNorm() > 0.0)
	{
		m_tempInC = (450 * getRPMNorm()) * overHeatCount();
	}
	else
	{
		m_tempInC = 0.0;
	}

	return m_tempInC;
}

double Engine::overHeatCount()
{
	//Overheating of the engine due to overspeed
	bool CounterGo = false;
	bool CounterBack = false;
	
	//Heat-Counter auskommentiert, da Heat-Failure f�r F-35 aktuell unbekannt ;-)
	/*
	if ((((m_state.m_mach > 1.17) && (m_state.m_mach <= 1.25)) && (m_state.m_airDensity > 1.112)) || (((m_state.m_mach > 1.40) && (m_state.m_mach <= 1.55)) && ((m_state.m_airDensity > 0.9093) && (m_state.m_airDensity <= 1.112))) || (((m_state.m_mach > 1.70) && (m_state.m_mach <= 1.9)) && ((m_state.m_airDensity > 0.6601) && (m_state.m_airDensity < 0.9093))) || (((m_state.m_mach > 2.2) && (m_state.m_mach <= 2.3)) && (m_state.m_airDensity < 0.6601)))
	{
		m_heatOne = 0.00001;
		CounterGo = true;
	}
	else if (((m_state.m_mach > 1.25) && (m_state.m_airDensity > 1.112)) || ((m_state.m_mach > 1.55) && ((m_state.m_airDensity > 0.9093) && (m_state.m_airDensity <= 1.112))) || ((m_state.m_mach > 1.90) && ((m_state.m_airDensity > 0.6601) && (m_state.m_airDensity < 0.9093))) || ((m_state.m_mach > 2.3) && (m_state.m_airDensity < 0.6601)))
	{
		m_heatOne = 0.00005;
		CounterGo = true;
	}
	else if ((m_state.m_mach < 1.0) && (m_heatTimerUP > 1) && (m_heatFailure == false))
	{
		CounterBack = true;
	}

	if (CounterGo == true)
	{
		m_heatTimerUP++;
	}
	else if ((CounterBack == true) && (CounterGo == false) && (m_heatTimerUP > m_heatTimerDOWN))
	{
		m_heatTimerDOWN++;
	}

	m_heatTwo = (m_heatOne * (m_heatTimerUP - m_heatTimerDOWN)) + 1.0;
	*/
	m_heatTwo = 1.0;
	return m_heatTwo;
}

double Engine::overHeat()
{
	if ((tempInC() > 1045.0) && (tempInC() <= 1250.0))
	{
		m_overHeat = 0.5;
	}
	else if ((tempInC() > 1250.0) && (tempInC() <= 1350.0))
	{
		m_overHeat = 0.75;
	}
	else if (tempInC() > 1350.0)
	{
		m_overHeat = 1.0;
		//m_input.m_engine_start = 0.0;
		//m_input.m_engine_stop = 0.0;
		m_heatFailure = true;
	}
	else if (((m_heatFailure == true) || (m_needRepair == true)) && (m_input.getEngineStart1() == 1.0))
	{
		//m_input.m_engine_start = 2.0;
		//m_input.m_engine_stop = 2.0;
		m_needRestart = true;
	}
	else
	{
		m_overHeat = 0.0;
	}

	return m_overHeat;
}

double Engine::overHeatInd()
{
	if (m_input.getElectricSystem() == 1.0)
	{
		if ((overHeat() >= 0.75) || (m_heatFailure == true))
		{
			m_overHeatInd = 1.0;
		}
		else
		{
			m_overHeatInd = 0.0;
		}
	}
	else
	{
		m_overHeatInd = 0.0;
	}
	return m_overHeatInd;
}

double Engine::overSpeedInd()
{
	if (m_input.getElectricSystem() == 1.0)
	{
		if (((m_state.m_mach > 1.114) && (m_state.m_airDensity > 1.112)) || ((m_state.m_mach > 1.25) && ((m_state.m_airDensity > 0.9093) && (m_state.m_airDensity <= 1.112))) || ((m_state.m_mach > 1.60) && ((m_state.m_airDensity > 0.6601) && (m_state.m_airDensity < 0.9093))) || ((m_state.m_mach > 2.2) && (m_state.m_airDensity < 0.6601)))
		{
			m_overSpeedInd = 1.0;
		}
		else
		{
			m_overSpeedInd = 0.0;
		}
	}
	else
	{
		m_overSpeedInd = 0.0;
	}

	return m_overSpeedInd;
}

//-----------------Repair and REset damages due to overheat or overspeed--------------------------------

void Engine::repairHeatDamage()
{
	m_overHeat = 0.0;
	m_overHeatInd = 0.0;
	//m_input.m_engine_start = 0.0;
	//m_input.m_engine_stop = 0.0;
	m_heatFailure = false;
	m_needRepair = false;
	m_heatTimerDOWN = 0;
	m_heatTimerUP = 0;

}

void Engine::restartNeeded()
{
	if ((m_input.getEngineStart1() == 0.0) && (m_heatFailure == false) && (m_needRepair == false) && (m_needRestart == true))
	{
		m_needRestart = false;
	}
}




