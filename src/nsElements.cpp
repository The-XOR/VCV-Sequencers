#include "../include/Nordschleife.hpp"
#include "../include/nordschleifeUI.hpp"

int NordschleifeCar::CarLed[NORDCARS] = {Nordschleife::LightIds::LOTUS_LED, Nordschleife::LightIds::BRABHAM_LED, Nordschleife::LightIds::FERRARI_LED, Nordschleife::LightIds::HESKETH_LED};

void NordschleifeCar::stepTrig()
{
	stepTrigger.trigger(PULSE_TIME);
	ledStepTrigger.trigger(0.1);
	pNord->outputs[Nordschleife::CAR_TRIG + myID].value = LVL_ON;
	pNord->lights[Nordschleife::TRIG_LED+myID].value = LED_ON;
}

bool NordschleifeCar::process(float deltaTime)
{
	bool step_moved = false;
	if(!lapPulse.process(deltaTime))
	{
		pNord->outputs[Nordschleife::CAR_LAP+myID].value = LVL_OFF;
	}
	if(!ledLapPulse.process(deltaTime))
		pNord->lights[Nordschleife::LAP_LED + myID].value = LED_OFF;

	if(!stepTrigger.process(deltaTime))
		pNord->outputs[Nordschleife::CAR_TRIG + myID].value = LVL_OFF;
	
	if(!ledStepTrigger.process(deltaTime))
		pNord->lights[Nordschleife::TRIG_LED+myID].value = LED_OFF;
	

	if(resetTrig.process(pNord->inputs[Nordschleife::CAR_RESET+myID].value))
	{
		reset();
		step_moved = true;
	} else if(!pNord->inputs[Nordschleife::CAR_CLOCK + myID].isConnected())
	{
		NordschleifeStep::Mute(pNord, myID);
		step_moved = true;
	} else
	{
		int clk = clockTrigger.process(pNord->inputs[Nordschleife::CAR_CLOCK + myID].value); // 1=rise, -1=fall
		if(clk == 1)
		{
			step_moved = true;
			stopWatch = 0.f;
			int nextStep = move_next();
			if(!inPit())
			{
				pNord->steps[playingStep].beginPulse(pNord, myID, lastPulseDuration, nextStep);
			}
			playingStep = nextStep;
		} else
		{			
			if(clk == -1)
			{
				lastPulseDuration = stopWatch;
				if(!pitstop)
				{
					NordschleifeStep::StepMode m = NordschleifeStep::EndPulse(pNord, myID);
					if(m == NordschleifeStep::StepMode::Reset)		// reset step?
					{
						playingStep = curStepCounter = startGrid;
					}
				}
			} else
			{
				stopWatch += deltaTime; // tempo trascorso dall'inizio del clock
				if(!pitstop)
					NordschleifeStep::Process(pNord, myID, deltaTime);
			}
		}
	}

	return step_moved;
}

bool NordschleifeCar::inPit()
{
	if(strategyEvery > 0)
	{
		if((lapCounter - pitStopCounter) == strategyEvery)
			pitstop = true;
		else if((lapCounter - pitStopCounter) == strategyEvery + strategyFor)
		{
			pitStopCounter = lapCounter;
			pitstop = false;
		}
	} else
	{
		pitstop = false;
		pitStopCounter = 0;
	}

	return pitstop;
}

void NordschleifeCar::onCollision()
{
	if(collision == carRnd)
		onCollision((CarCollision)int(random::uniform() * carRnd));
	else
		onCollision(collision);
}

void NordschleifeCar::onCollision(CarCollision n)
{
	switch(n)
	{
		case carRnd:
		case carIgnore:
			break;

		case car90left:
			angle = (angle + 1) % 4;
			break;

		case car90right:
			if(--angle < 0)
				angle = 3;
			break;

		case carInvert:
			angle = (angle + 2) % 4;
			break;

		case nextPath:
			path = (path + 1) % NORDPATHS;
			break;

		case prevPath:
			if(--path < 0)
				path = NORDPATHS - 1;
			break;

		case randomPath:
			path = int(random::uniform() * NORDPATHS);
			break;
	}
}


void NordschleifeCar::pulseTrig()
{
	lapPulse.trigger(PULSE_TIME);
	ledLapPulse.trigger(0.1);
	pNord->outputs[Nordschleife::CAR_LAP + myID].value = LVL_ON;
	pNord->lights[Nordschleife::LAP_LED +myID].value = LED_ON;
}

int NordschleifeCar::get_next_step()
{
	switch(direction)
	{
		case carForward:
			if(++curStepCounter > stepTo)
				curStepCounter = stepFrom;
			break;

		case carBackward:
			if(--curStepCounter < stepFrom)
				curStepCounter = stepTo;
			break;

		case carAlternate:
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
			break;

		case carRandom:
			curStepCounter = int((stepTo - stepFrom) * random::uniform()) + stepFrom;
			break;

		case carBrownian:
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
		break;
	}

	return Nordschleife::paths[path][pNord->rotation[angle][curStepCounter]];
}

int NordschleifeCar::move_next()
{
	int rv;
	for(int k = 0; k< NORDSTEPS; k++)
	{
		rv = get_next_step();
		if(pNord->steps[rv].mode != NordschleifeStep::StepMode::Skip)
			break;
	} 

	totalCounter++;
	lapCounter = totalCounter / NORDSTEPS;
	if((totalCounter % NORDSTEPS) == 0)
		pulseTrig();

	return rv;
}

void NordschleifeCar::reset()
{
	lapPulse.reset();
	ledLapPulse.reset();
	moving_bwd = false;
	playingStep= curStepCounter=startGrid;
	totalCounter = lapCounter = pitStopCounter = 0;
	pitstop = false;
	pNord->outputs[Nordschleife::CAR_GATE + myID].value = LVL_OFF;
	NordschleifeStep::Mute(pNord, myID);
}

void NordschleifeCar::Init(Nordschleife *p, int id)
{
	pNord = p;
	myID = id;
	myIDstr = std::to_string(id);
	name = Nordschleife::carNames[myID];
	init();
}

void NordschleifeCar::init()
{
	stopWatch = 0.f;
	lastPulseDuration = 0.f;
	angle =path = stepFrom = startGrid = 0;
	stepTo = NORDSTEPS - 1;
	direction = carForward;
	collision = carIgnore;
	strategyEvery = 0;
	strategyFor = 1;
	offset=0;
	reset();
}

int NordschleifeStep::selectedByCar[NORDCARS] = {STEP_RESET, STEP_RESET, STEP_RESET, STEP_RESET};
inline void NordschleifeStep::Mute(Nordschleife *pNord, int carID) {pNord->steps[NordschleifeStep::selectedByCar[carID]].mute(pNord, carID); }
inline NordschleifeStep::StepMode NordschleifeStep::EndPulse(Nordschleife *pNord, int carID) { return NordschleifeStep::selectedByCar[carID] != STEP_RESET ? pNord->steps[NordschleifeStep::selectedByCar[carID]].endPulse(pNord, carID) : Off; }
inline void NordschleifeStep::Process(Nordschleife *pNord, int carID, float deltaTime) { if(NordschleifeStep::selectedByCar[carID] != STEP_RESET) pNord->steps[NordschleifeStep::selectedByCar[carID]].process(pNord, carID, deltaTime); }

void NordschleifeStep::beginPulse(Nordschleife *pNord, int carID, float lastPulseDuration, int nextStep)
{
	if(NordschleifeStep::selectedByCar[carID] != STEP_RESET)
		pNord->lights[NordschleifeCar::CarLed[carID] + NordschleifeStep::selectedByCar[carID]].value = LED_OFF;

	pNord->outputs[Nordschleife::CAR_POSITION+carID].setVoltage(SEMITONE * myID);

	if(mode == Skip) // skip: lo step "non esiste", viene completamente ignorato
	{
		NordschleifeStep::selectedByCar[carID] = STEP_RESET;	//in questo momemnto, questa automobile non ha nessuno step associato
		return;
	}
	// se non si e' in skip, il led viene acceso e l'eventuale segnale di "current step" generato
	
	NordschleifeStep::selectedByCar[carID] = myID;	//questo step viene associato all'automobile
	pNord->lights[NordschleifeCar::CarLed[carID] + myID].value = LED_ON;
	if(outA == carID)
	{
		pNord->outputs[Nordschleife::OUT_A + myID].value = LVL_ON;
	}
	if(outB == carID)
	{
		stepPulseB.trigger(PULSE_TIME);
		pNord->outputs[Nordschleife::OUT_B + myID].value = LVL_ON;
	}

	if(trigger)
	{
		pNord->cars[carID].stepTrig();
	}

	if(mode == Off)  //step off: non suona, non ripete
	{
		pNord->outputs[Nordschleife::CAR_GATE + carID].value = LVL_OFF;
		return;
	}

	// se ne' skip ne' off, lo step e' valido. rimane da vedere se il Fato vuole che suoni o no
	playing[carID] = int(100 * random::uniform()) < probability;
	if(playing[carID])
	{
		// il fato e' stato benevolo. Il voltaggio dello step viene prodotto in uscita
		startVoltage[carID] = SEMITONE*pNord->cars[carID].offset + SEMITONE*offset + pNord->cvs.TransposeableValue(pNord->params[Nordschleife::VOLTAGE_1 + myID].getValue());
		pNord->outputs[Nordschleife::CAR_CV + carID].setVoltage(startVoltage[carID]);
		// se pero' lo step e' in Reset, NON viene generato il segnale di gate
		pNord->outputs[Nordschleife::CAR_GATE + carID].setVoltage(mode == Reset ? LVL_OFF : LVL_ON);
		repCount[carID] = repeats;
		pulseDuration[carID] = lastPulseDuration;
		repeat_gateStatus[carID] = true;  // attualmente, gate e' ON
		elapsedTime[carID] = stopWatch[carID] = 0.f; // tempo trascorso dall'ultima ripetizione
		slideToVoltage[carID] = SEMITONE*pNord->cars[carID].offset + SEMITONE * offset + pNord->cvs.TransposeableValue(pNord->params[Nordschleife::VOLTAGE_1 + nextStep].getValue());
	}
}

NordschleifeStep::StepMode NordschleifeStep::endPulse(Nordschleife *pNord, int carID)  // ritorna true se lo step e' in modalita Reset
{
	pNord->outputs[Nordschleife::OUT_A + myID].value = LVL_OFF;

	if(mode != Skip) // skip: lo step "non esiste", viene completamente ignorato
	{
		if(playing[carID])  // se lo step sta effettivamente suonando, viene chiuso il gate
		{
			if(mode != Reset && mode != Legato && mode != Slide)
			{
				pNord->outputs[Nordschleife::CAR_GATE + carID].setVoltage(LVL_OFF);
			}
			playing[carID] = false;  //play no more
		}
	} 
	return mode;
}

void NordschleifeStep::process(Nordschleife *pNord, int carID, float deltaTime)
{
	elapsedTime[carID] += deltaTime;
	if(!stepPulseB.process(deltaTime))
		pNord->outputs[Nordschleife::OUT_B + myID].value = LVL_OFF;

	float timeSlice = pulseDuration[carID] / (1 + repeats);	// quanto dura una singola ripetizione
	if(repeats > 1 && repCount[carID] > 0 && timeSlice > 0)
	{
		// quanto e' passato dall'ultimo cambio di stato?
		stopWatch[carID] += deltaTime;
		if(stopWatch[carID] >= timeSlice)  // e' ora di cambiare stato?
		{
			if(repeat_gateStatus[carID])   // se gate era ON, spegne il gate e conclude il ciclo attuale
			{
				pNord->outputs[Nordschleife::CAR_GATE + carID].setVoltage(LVL_OFF);
				repCount[carID]--;
				repeat_gateStatus[carID] = false;
			} else
			{
				// se gate era OFF, rimette a ON
				pNord->outputs[Nordschleife::CAR_GATE + carID].setVoltage(LVL_ON);
				repeat_gateStatus[carID] = true;
			}
			stopWatch[carID] = 0.f; // inizia un nuovo ciclo
		}
	}

	if(mode == Slide && pulseDuration[carID] > 0)
	{
		float v = startVoltage[carID] + (elapsedTime[carID] / pulseDuration[carID]) * (slideToVoltage[carID] - startVoltage[carID]);
		pNord->outputs[Nordschleife::CAR_CV + carID].setVoltage(v);
	}
}

void NordschleifeStep::mute(Nordschleife *pNord, int carID)
{
	if(NordschleifeStep::selectedByCar[carID] != STEP_RESET)
	{
		endPulse(pNord, carID);
		pNord->lights[NordschleifeCar::CarLed[carID] + NordschleifeStep::selectedByCar[carID]].value = LED_OFF;
	}
	NordschleifeStep::selectedByCar[carID] = STEP_RESET;
	//NordschleifeStep::selectedByCar[carID] = pNord->cars[carID].startGrid;
	//pNord->lights[NordschleifeCar::CarLed[carID] + NordschleifeStep::selectedByCar[carID]].value = LED_ON;
}