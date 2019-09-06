#include "../include/Nordschleife.hpp"
#include "../include/nordschleifeUI.hpp"

int NordschleifeCar::process(float deltaTime)
{
	if(lapPulse.process(deltaTime))
	{
		pNord->outputs[Nordschleife::CAR_LAP+myID].value = LVL_ON;
	} else
	{
		pNord->outputs[Nordschleife::CAR_LAP+myID].value = LVL_OFF;
	}

	if(resetTrig.process(pNord->inputs[Nordschleife::CAR_RESET+myID].value))
	{
		reset();
	} else if(!pNord->inputs[Nordschleife::CAR_CLOCK + myID].isConnected())
	{
		ledOff();
	} else
	{
		int clk = clockTrigger.process(pNord->inputs[Nordschleife::CAR_CLOCK + myID].value); // 1=rise, -1=fall
		if(clk == 1)
		{
			if(repetitions-- < 1)
			{
				ledOff();
				step_n = move_next();
				repetitions = pNord->steps[step_n].beginPulse(pNord, myID);
				ledOn();
			}
			return step_n;

		} else if(step_n != STEP_RESET)
		{
			if(pNord->steps[step_n].endPulse(pNord, myID, deltaTime, clk == -1))
				step_n = curStepCounter = STEP_RESET;
		}
	}

	return STEP_RESET;
}

void NordschleifeCar::onCollision()
{
	switch(collision)
	{
		case carIgnore:
			break;

		case car90left: 
			angle = (angle+1) % 4;
			break;
		
		case car90right: 
			if(--angle < 0)
				angle=3;
			break;

		case carInvert: 
			angle = (angle+2) % 4;
			break;
	}
}

void NordschleifeCar::pulseTrig()
{
	lapPulse.trigger(PULSE_TIME);
	pNord->outputs[Nordschleife::CAR_LAP+myID].value = LVL_ON;
}

int NordschleifeCar::get_next_step()
{
	switch(direction)
	{
		case carForward:
			if(curStepCounter == STEP_RESET)
				curStepCounter = stepFrom;
			else if(++curStepCounter > stepTo)
				curStepCounter = stepFrom;
			break;

		case carBackward:
			if(curStepCounter == STEP_RESET)
				curStepCounter = stepTo;
			else if(--curStepCounter < stepFrom)
				curStepCounter = stepTo;
			break;

		case carAlternate:
			if(curStepCounter == STEP_RESET)
				curStepCounter = stepFrom;
			else
			{
				if(moving_bwd)
				{
					if(--curStepCounter < stepFrom) // siamo a battuta verso sx
					{
						curStepCounter = stepTo;
						moving_bwd = false;
					}
				} else
				{
					if(++curStepCounter > stepTo) // siamo a battuta verso dx
					{
						curStepCounter = stepTo - 1;
						moving_bwd = true;
					}
				}
			}
			break;

		case carRandom:
			curStepCounter = int((stepTo - stepFrom) * random::uniform()) + stepFrom;
			break;

		case carBrownian:
		{
			if(curStepCounter == STEP_RESET)
				curStepCounter = stepFrom;
			else
			{
				switch(int(random::uniform() * 3))
				{
					case 1:
						if(++curStepCounter > stepTo)
							curStepCounter = stepFrom;
						break;

					case 2:
						if(--curStepCounter < stepFrom)
							curStepCounter = stepTo;
						break;

					case 3: // aripetemo
						break;
				}
			}
		}
		break;
	}

	return Nordschleife::paths[path][pNord->rotation[angle][curStepCounter]];
}

int NordschleifeCar::move_next()
{
	if(pNord->steps[step_n].repCount > 0)
	{
		return Nordschleife::paths[path][pNord->rotation[angle][curStepCounter]];
	}

	int rv;
	for(int k = 0; k< NORDSTEPS; k++)
	{
		rv = get_next_step();
		if(pNord->steps[rv].mode != NordschleifeStep::StepMode::Skip)
			break;
	} 

	lapCounter++;
	return rv;
}

void NordschleifeCar::reset()
{
	lapPulse.reset();
	ledOff();
	moving_bwd = false;
	step_n=curStepCounter=STEP_RESET;
	repetitions = lapCounter = 0;
	pNord->outputs[Nordschleife::CAR_GATE + myID].value = LVL_OFF;
}

inline void NordschleifeCar::ledOff()
{
	if(step_n != STEP_RESET)
	{
		pNord->lights[lightid + step_n].value = LED_OFF;
		step_n = STEP_RESET;
	}
}

inline void NordschleifeCar::ledOn() { if(step_n != STEP_RESET) pNord->lights[lightid + step_n].value = LED_ON; }

void NordschleifeCar::Init(Nordschleife *p, int id, int lid)
{
	pNord = p;
	lightid = lid;
	myID = id;
	myIDstr = std::to_string(id);
	name = Nordschleife::carNames[myID];
	init();
}

void NordschleifeCar::init()
{
	path = stepFrom = 0;
	stepTo = NORDSTEPS - 1;
	direction = carForward;
	collision = carIgnore;
	reset();
}

int NordschleifeStep::beginPulse(Nordschleife *pNord, int carID) 
{
	if(mode == Skip)
		return repCount = 0;
	else if(mode == Off)
	{
		pNord->outputs[Nordschleife::CAR_GATE + carID].value = LVL_OFF;
		return repCount = 0;
	}

	if(outA == carID)
	{
		stepPulseA.trigger(PULSE_TIME);
		pNord->outputs[Nordschleife::OUT_A + myID].value = LVL_ON;
	}
	if(outB == carID)
	{
		stepPulseB.trigger(PULSE_TIME);
		pNord->outputs[Nordschleife::OUT_B + myID].value = LVL_ON;
	}	

	pNord->outputs[Nordschleife::CAR_CV + carID].value = pNord->cvs.TransposeableValue(pNord->params[Nordschleife::VOLTAGE_1 + myID].value);
	if(mode == Reset)
	{
		pNord->outputs[Nordschleife::CAR_GATE + carID].value = LVL_OFF;
		return repCount = 0;
	}

	if(int(100 * random::uniform()) < probability)
		pNord->outputs[Nordschleife::CAR_GATE + carID].value = LVL_ON;

	return repCount = repeats;
}

bool NordschleifeStep::endPulse(Nordschleife *pNord, int carID, float deltaTime, bool end) // true if reset
{
	if(mode == Off || mode == Skip)
		return false;

	pNord->outputs[Nordschleife::OUT_A + myID].value = stepPulseA.process(deltaTime) ? LVL_ON : LVL_OFF;
	pNord->outputs[Nordschleife::OUT_B + myID].value = stepPulseB.process(deltaTime) ? LVL_ON : LVL_OFF;
	if(end)
	{
		if(mode != Legato)
			pNord->outputs[Nordschleife::CAR_GATE + carID].value = LVL_OFF;	
		repCount--;
	}

	return mode == Reset;
}
