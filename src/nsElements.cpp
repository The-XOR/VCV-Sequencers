#include "../include/Nordschleife.hpp"
#include "../include/nordschleifeUI.hpp"

#define STEP_RESET		-200
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
	} else
	{
		int clk = clockTrigger.process(pNord->inputs[Nordschleife::CAR_CLOCK + myID].value); // 1=rise, -1=fall
		if(clk == 1)
		{
			ledOff();
			step_n = move_next();
			beginPulse(false);
			ledOn();
		} else if(clk == -1)
			endPulse();
	}

	return step_n;
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

void NordschleifeCar::beginPulse(bool silent)
{
//	pNord->outputs[Nordschleife::CAR_CV + myID].value = pNord->cvs.TransposeableValue(pNord->params[Nordschleife::VOLTAGE_1 + step_n].value);
//	pNord->outputs[Nordschleife::CAR_GATE + myID].value = silent ? LVL_OFF : LVL_ON;
}

void NordschleifeCar::endPulse()
{
	//pNord->outputs[Nordschleife::CAR_GATE + myID].value = LVL_OFF;
}

int NordschleifeCar::move_next()
{
	switch(direction)
	{
		case carForward:
			if(curStepCounter == STEP_RESET)
				curStepCounter=stepFrom;
			else if(++curStepCounter > stepTo)
				curStepCounter = stepFrom;
			break;

		case carBackward:
			if(curStepCounter == STEP_RESET)
				curStepCounter=stepTo;
			else if(--curStepCounter < stepFrom)
				curStepCounter = stepTo;
			break;

		case carAlternate:
			if(curStepCounter == STEP_RESET)
				curStepCounter=stepFrom;
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
				curStepCounter=stepFrom;
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

	lapCounter++;
	return Nordschleife::paths[path][pNord->rotation[angle][curStepCounter]];
}

void NordschleifeCar::ledOff()
{
	if(step_n != STEP_RESET)
	{
		pNord->lights[lightid+step_n].value = LED_OFF;
		step_n=STEP_RESET;
	}
}

void NordschleifeCar::ledOn()
{
	pNord->lights[lightid+step_n].value = LED_ON;
}

void NordschleifeCar::reset()
{
	lapPulse.reset();
	endPulse();
	ledOff();
	moving_bwd = false;
	step_n=curStepCounter=STEP_RESET;
	lapCounter = 0;
}

void NordschleifeCar::Init(Nordschleife *p, int id, int lid)
{
	pNord = p;
	lightid = lid;
	myID = id;
	myIDstr = std::to_string(id);
	name = Nordschleife::carNames[myID];
	path = stepFrom = 0;
	stepTo = NORDSTEPS-1;
	direction = carForward;
	collision = carIgnore;
	reset();
}