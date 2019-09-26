#include "../include/Nordschleife.hpp"
#include "../include/nordschleifeUI.hpp"

#define SETCARFIELD(fieldName, fieldValue) {if(GangBang()) for(int k=0; k<NORDCARS;k++) cars[k].fieldName = fieldValue; else cars[selectedCar].fieldName = fieldValue; }
#define SETSTEPFIELD(fieldName, fieldValue) {if(GangBang()) for(int k=0; k<NORDSTEPS;k++) steps[k].fieldName = fieldValue; else steps[selectedStep].fieldName = fieldValue; }
#define TO_STR(i) {return std::to_string(i);}

std::vector<std::string> Nordschleife::carNames = {"Lotus", "Brabham", "Ferrari", "Hesketh"};
int Nordschleife::paths[NORDPATHS][64] =
{
	{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63},
	{0,1,2,3,4,5,6,7,15,14,13,12,11,10,9,8,16,17,18,19,20,21,22,23,31,30,29,28,27,26,25,24,32,33,34,35,36,37,38,39,47,46,45,44,43,42,41,40,48,49,50,51,52,53,54,55,63,62,61,60,59,58,57,56},
	{0,8,16,24,32,40,48,56,1,9,17,25,33,41,49,57,2,10,18,26,34,42,50,58,3,11,19,27,35,43,51,59,4,12,20,28,36,44,52,60,5,13,21,29,37,45,53,61,6,14,22,30,38,46,54,62,7,15,23,31,39,47,55,63},
	{56,48,40,32,24,16,8,0,1,9,17,25,33,41,49,57,58,50,42,34,26,18,10,2,3,11,19,27,35,43,51,59,60,52,44,36,28,20,12,4,5,13,21,29,37,45,53,61,62,54,46,38,30,22,14,6,7,15,23,31,39,47,55,63},
	{0,1,2,3,4,5,6,7,15,23,31,39,47,55,63,62,61,60,59,58,57,56,48,40,32,24,16,8,9,10,11,12,13,14,22,30,38,46,54,53,52,51,50,49,41,33,25,17,18,19,20,21,29,37,45,44,43,42,34,26,27,28,36,35},
	{56,48,57,40,49,58,32,41,50,59,24,33,42,51,60,16,25,34,43,52,61,8,17,26,35,44,53,62,0,9,18,27,36,45,54,63,1,10,19,28,37,46,55,2,11,20,29,38,47,3,12,21,30,39,4,13,22,31,5,14,23,6,15,7},
	{0,7,1,6,2,5,3,4,8,15,9,14,10,13,11,12,16,23,17,22,18,21,19,20,24,31,25,30,26,29,27,28,32,39,33,38,34,37,35,36,40,47,41,46,42,45,43,44,48,55,49,54,50,53,51,52,56,63,57,62,58,61,59,60},
	{0,56,8,48,16,40,24,32,1,57,9,49,17,41,25,33,2,58,10,50,18,42,26,34,3,59,11,51,19,43,27,35,4,60,12,52,20,44,28,36,5,61,13,53,21,45,29,37,6,62,14,54,22,46,30,38,7,63,15,55,23,47,31,39},
	{0,1,2,3,4,5,6,7,15,23,31,39,47,55,63,62,61,60,59,58,57,56,48,40,32,24,16,8,9,10,11,12,13,14,22,30,38,46,54,53,52,51,50,49,41,33,25,17,18,19,20,21,29,37,45,44,43,42,34,26,27,28,36,35},
	{0,4,1,5,2,6,3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63,58,62,57,61,56,60,48,52,40,44,32,36,24,28,16,20,8,12,9,13,10,14,18,22,26,30,34,38,42,46,50,54,49,53,41,45,33,37,25,29,17,21},
	{0,7,9,14,18,21,27,28,35,36,42,45,49,54,56,63,1,6,10,13,19,20,43,44,50,53,57,62,8,15,17,22,26,29,34,37,41,46,48,55,2,5,11,12,51,52,58,61,16,23,25,30,33,38,40,47,3,4,59,60,24,31,32,39},
	{0,7,3,4,9,14,10,13,16,23,19,20,25,30,26,29,32,39,35,36,41,46,42,45,48,55,51,52,57,58,61,62,1,6,11,12,17,22,27,28,33,38,43,44,49,54,59,60,2,5,8,15,18,21,24,31,34,37,40,47,50,53,56,63}
};

void Nordschleife::process(const ProcessArgs &args)
{
	if(masterReset.process(params[M_RESET].value + inputs[MRESET_IN].value))
	{
		reset();
	} else
	{
		if(pWidget != NULL && rndTrigger.process(inputs[RANDOMIZONE].value))
			randrandrand();

		if(lazyCheck++ % 20 == 0)
		{
			step_select();
			car_select();
			data_entry();
		}

		float rec_smp;
		int rec_step;
		if(cvs.IsRecAvailable(&rec_smp, &rec_step))
		{
			pWidget->SetValue(Nordschleife::VOLTAGE_1 + rec_step, rec_smp);
		}

		// race starts here!
		float deltaTime = 1.0 / args.sampleRate;
		for(int k = 0; k < NORDCARS; k++)
		{
			if(cars[k].process(deltaTime) && NordschleifeStep::Collision(k))
				cars[k].onCollision();
		}
	}
}

void Nordschleife::car_select()
{
	for(int k = 0; k < NORDCARS; k++)
	{
		if(carSelectTrigger[k].process(params[CAR_SELECT + k].getValue()))
		{
			setCar(k);
			break;
		}
	}
}

void Nordschleife::data_entry()
{
	if(consumeKey(GLFW_KEY_ENTER))
	{
		toggleDataEntryMode();
		return;
	} else if(consumeKey(GLFW_KEY_KP_0) || gang != (params[GANGBANG].getValue() > 0))
	{
		setGang(!gang);
		return;
	}

	int move = 0;
	if(consumeKey(GLFW_KEY_KP_MULTIPLY) || rightKey.process(params[DATAENTRY_RIGHT].getValue()))
		move = GLFW_KEY_KP_MULTIPLY;
	else if(consumeKey(GLFW_KEY_KP_DIVIDE) || leftKey.process(params[DATAENTRY_LEFT].getValue()))
		move = GLFW_KEY_KP_DIVIDE;
	else if(consumeKey(GLFW_KEY_KP_SUBTRACT) || upKey.process(params[DATAENTRY_UP].getValue()))
		move = GLFW_KEY_KP_SUBTRACT;
	else if(consumeKey(GLFW_KEY_KP_ADD) || downKey.process(params[DATAENTRY_DOWN].getValue()))
		move = GLFW_KEY_KP_ADD;


	if(moveByStep())
	{
		int r = selectedStep / 8;
		int c = selectedStep % 8;
		int curStep = selectedStep;
		switch(move)
		{
			case GLFW_KEY_KP_DIVIDE:
				if(--curStep < r * 8) curStep = r * 8 + 7;
				setStep(curStep);
				break;
			case GLFW_KEY_KP_MULTIPLY:
				if(++curStep > 7 + r * 8) curStep = r * 8;
				setStep(curStep);
				break;
			case GLFW_KEY_KP_SUBTRACT:
				curStep -= 8;
				if(curStep < 0) curStep = 56 + c;
				setStep(curStep);
				break;
			case GLFW_KEY_KP_ADD:
				curStep += 8;
				if(curStep > 63) curStep = c;
				setStep(curStep);
				break;
		}
	} else // move by field?
	{
		switch(move)
		{
			case GLFW_KEY_KP_SUBTRACT:
			case GLFW_KEY_KP_ADD:
				display->moveField(move);
				break;

			case GLFW_KEY_KP_DIVIDE:
				nsFields[display->getCurField()].dec();
				break;
			case GLFW_KEY_KP_MULTIPLY:
				nsFields[display->getCurField()].inc();
				break;
		}
	}
}

void Nordschleife::step_select()
{
	for(int k = 0; k < NORDSTEPS; k++)
	{
		if(stepSelectTrigger[k].process(params[STEPSELECT_1 + k].getValue()))
		{
			setStep(k);
			break;
		}
	}
}

void Nordschleife::setAllCurrent()
{
	float v = params[VOLTAGE_1 + selectedStep].value;
	for(int k = 0; k < NORDSTEPS; k++)
	{
		pWidget->SetValue(VOLTAGE_1 + k, v);
	}
}

void Nordschleife::on_loaded()
{
	load();
	cvs.Init(pWidget);
}

void Nordschleife::load()
{
	reset();
}

void Nordschleife::randrandrand()
{
	if(theRandomizer & NordschleifeWidget::RANDOMIZE_PITCH)
		randrandrand(0);
	else if(theRandomizer & NordschleifeWidget::RANDOMIZE_MODE)
		randrandrand(1);
	else if(theRandomizer & NordschleifeWidget::RANDOMIZE_PROBABILITY)
		randrandrand(2);
	else if(theRandomizer & NordschleifeWidget::RANDOMIZE_REPETITIONS)
		randrandrand(3);
	else if(theRandomizer & NordschleifeWidget::RANDOMIZE_DIRECTION)
		randrandrand(4);
	else if(theRandomizer & NordschleifeWidget::RANDOMIZE_PATH)
		randrandrand(5);
	else if(theRandomizer & NordschleifeWidget::RANDOMIZE_ONCRASH)
		randrandrand(6);
	else if(theRandomizer & NordschleifeWidget::RANDOMIZE_ANGLE)
		randrandrand(7);
	else if(theRandomizer & NordschleifeWidget::RANDOMIZE_OFFSET)
		randrandrand(8);
	else if(theRandomizer & NordschleifeWidget::RANDOMIZE_STEPOFFSET)
		randrandrand(9);

	if(theRandomizer & NordschleifeWidget::RANDOMIZE_LAQUALUNQUE)
	{
		randrandrand(int(random::uniform() * 10));
	}
}

void Nordschleife::randrandrand(int action)
{
	switch(action)
	{
		case 0:
			pWidget->std_randomize(Nordschleife::VOLTAGE_1, Nordschleife::VOLTAGE_1 + NORDSTEPS);
			break;

		case 1: //mode
			for(int k = 0; k < NORDSTEPS; k++)
				steps[k].mode = (NordschleifeStep::StepMode)int(random::uniform() * 6);
			break;

		case 2: //probabil
			for(int k = 0; k < NORDSTEPS; k++)
				steps[k].probability = int(random::uniform() * 101);
			break;

		case 3: //reps
			for(int k = 0; k < NORDSTEPS; k++)
				steps[k].repeats = 1 + int(random::uniform() * 8);
			break;

		case 4: //direc
			for(int k = 0; k < NORDCARS; k++)
				cars[k].direction = (NordschleifeCar::CarDirection)int(random::uniform() * 5);
			break;

		case 5: //path
			for(int k = 0; k < NORDCARS; k++)
				cars[k].path = int(random::uniform() * NORDPATHS);
			break;

		case 6: //crsh
			for(int k = 0; k < NORDCARS; k++)
				cars[k].collision = (NordschleifeCar::CarCollision)int(random::uniform() * 8);
			break;

		case 7: //angle
			for(int k = 0; k < NORDCARS; k++)
				cars[k].angle = int(random::uniform() * 4);
			break;

		case 8: //offset
			for(int k = 0; k < NORDCARS; k++)
				cars[k].offset = int(random::uniform() * 49) - 24;
			break;

		case 9: //step offset
			for(int k = 0; k < NORDSTEPS; k++)
				steps[k].offset = int(random::uniform() * 49) - 24;
			break;
	}
}

void Nordschleife::QuantizePitch()
{
	for(int k = 0; k < NORDSTEPS; k++)
		params[VOLTAGE_1 + k].value = pWidget->quantizePitch(VOLTAGE_1 + k, params[VOLTAGE_1 + k].value, cvs);
}

void Nordschleife::declareFields()
{
	// cars
	int row = 0;
	nsFields[NordschleifeFields::shlfDirection].set(0, row++, "Direction: ", 0, NordschleifeCar::CarDirection::NUM_CAR_DIRECTIONS-1,
													[this] {return cars[selectedCar].direction; },
													[this](int i) SETCARFIELD(direction, (NordschleifeCar::CarDirection)i),
													[this](int i) {static const char *m[] = {"Forward", "Backward", "Alternate", "Brownian", "Random"}; return m[i]; }
	);

	nsFields[NordschleifeFields::shlfPath].set(0, row++, "Path: ", 0, NORDPATHS-1, 
											   [this] {return cars[selectedCar].path; },
											   [this](int i) SETCARFIELD(path, i),
											[this](int i) {static const char *m[] = {"Tiergarten", "Karussell", "Adenauer Forst", "Metzgesfeld","Eiskurve", "Aremberg", "Flugplatz", "Hohe Acht", "Pflanzgarten", "Kallenhard","Steilstrecke","Eschbach"}; return m[i]; }
	);

	nsFields[NordschleifeFields::shlfCollision].set(0, row++, "On Crash: ", 0, NordschleifeCar::CarCollision::NUM_CAR_COLLISIONS-1, 
													[this] {return cars[selectedCar].collision; },
													[this](int i) SETCARFIELD(collision, (NordschleifeCar::CarCollision)i),
													[this](int i) {static const char *m[] = {"Ignore", "Invert", "90 left", "90 right", "Next path", "Prev path", "Rndm path", "???"}; return m[i]; }
	);

	nsFields[NordschleifeFields::shlfFrom].set(0, row++, "From: ", 0, NORDSTEPS-2,
											   [this] {return cars[selectedCar].stepFrom; },
											   [this](int i) {SETCARFIELD(stepFrom, i); if(!GangBang()) cars[selectedCar].stepTo = std::max(cars[selectedCar].stepTo, i); },
											   [this](int i) TO_STR(i + 1)
	);

	nsFields[NordschleifeFields::shlfTo].set(0, row++, "To: ", 0, NORDSTEPS-1,
											 [this] {return cars[selectedCar].stepTo; },
											 [this](int i) {SETCARFIELD(stepTo, i); if(!GangBang()) cars[selectedCar].stepFrom = std::min(cars[selectedCar].stepFrom, i); },
											 [this](int i) TO_STR(i + 1)
	);

	nsFields[NordschleifeFields::shlfStartGrid].set(0, row++, "Start: ", 0, NORDSTEPS-1,
													[this] {return cars[selectedCar].startGrid; },
													[this](int i) SETCARFIELD(startGrid, i),
													[this](int i) TO_STR(i + 1)
	);

	nsFields[NordschleifeFields::shlfAngle].set(0, row++, "Angle: ", 0, 3,
												[this] {return cars[selectedCar].angle; },
												[this](int i) SETCARFIELD(angle, i),
												[this](int i) {static const char *m[] = {"0", "90 L", "180", "90 R"}; return m[i]; }
	);

	nsFields[NordschleifeFields::shlfStrtgEvery].set(0, row++, "Pit every:", 0, 24,
													 [this] {return cars[selectedCar].strategyEvery; },
													 [this](int i) SETCARFIELD(strategyEvery, i),
													 [this](int i) TO_STR(i)
	);
	nsFields[NordschleifeFields::shlfStrtgFor].set(0, row++, "for: ", 1, 12,
												   [this] {return cars[selectedCar].strategyFor; },
												   [this](int i) SETCARFIELD(strategyFor, i),
												   [this](int i) TO_STR(i)
	);

	nsFields[NordschleifeFields::shlfOffset].set(0, row++, "Offset (st): ", -24, +24,
												 [this] {return cars[selectedCar].offset; },
												 [this](int i) SETCARFIELD(offset, i),
												 [this](int i) TO_STR(i)
	);

	// steps
	nsFields[NordschleifeFields::shlfStep].set(0, row, "Step #", 0, NORDSTEPS-1,
											   [this] {return selectedStep; },
											   [this](int i) {if(!GangBang()) setStep(i); },
											   [this](int i) TO_STR(i + 1),
											   true);

	row += 3;

	nsFields[NordschleifeFields::shlfMode].set(0, row++, "Mode: ", 0, NordschleifeStep::StepMode::NUM_STEP_MODE-1,
											   [this] {return steps[selectedStep].mode; }, 
											   [this](int i) SETSTEPFIELD(mode, (NordschleifeStep::StepMode)i),
												[this](int i) {static const char *m[] = {"Off", "On", "Skip", "Legato", "Slide", "Reset"}; return m[i]; }
	);
	nsFields[NordschleifeFields::shlfProbab].set(0, row++, "Probability: ", 1, 100,
												 [this] {return steps[selectedStep].probability; },
												 [this](int i) SETSTEPFIELD(probability, i),
												 [this](int i) TO_STR(i)
	);

	nsFields[NordschleifeFields::shlfRepeats].set(0, row++, "Reps: ", 1, 8,
												  [this] {return steps[selectedStep].repeats; },
												  [this](int i) SETSTEPFIELD(repeats, i),
												  [this](int i) TO_STR(i)
	);

	nsFields[NordschleifeFields::shlfStepOffset].set(0, row++, "Offs(st):", -24, +24,
													 [this] {return steps[selectedStep].offset; },
													 [this](int i) SETSTEPFIELD(offset, i),
													 [this](int i) TO_STR(i)
	);

	nsFields[NordschleifeFields::shlfOutA].set(0, row++, "Gate Out: ", 0, NORDCARS-1, 											   
											   [this] {return steps[selectedStep].outA; },
											   [this](int i) SETSTEPFIELD(outA, i),
											   [this](int i) {return Nordschleife::carNames[i];}
	);

	nsFields[NordschleifeFields::shlfOutB].set(0, row++, "Trig Out: ", 0, NORDCARS-1, 
											   [this] {return steps[selectedStep].outB; },
											   [this](int i) SETSTEPFIELD(outB, i),
											   [this](int i) {return Nordschleife::carNames[i];}
	);

	nsFields[NordschleifeFields::shlfTrigger].set(0, row++, "Car trigger: ", 0, 1, 
											   [this] {return steps[selectedStep].trigger ? 1 : 0; },
											   [this](int i) {steps[selectedStep].trigger = i > 0;},
											   [this](int i) {return i ? "Yes" : "No";}
	);

}

void Nordschleife::init_tables()
{
	for(int n = 0; n < NORDSTEPS; n++)
	{
		int r = n / 8;
		int c = n % 8;
		rotation[0][n] = n;

		rotation[1][n] = 56 - c * 8 + r;

		r = rotation[1][n] / 8;
		c = rotation[1][n] % 8;
		rotation[2][n] = 56 - c * 8 + r;

		r = rotation[2][n] / 8;
		c = rotation[2][n] % 8;
		rotation[3][n] = 56 - c * 8 + r;
	}
}

TransparentWidget *Nordschleife::createDisplay(Vec pos)
{
	display = createWidget<nordDisplay>(pos);
	display->setModule(this);
	declareFields();
	return display;
}

NordschleifeWidget::NordschleifeWidget(Nordschleife *module)
{
	CREATE_PANEL(module, this, 60, "res/modules/Nordschleife.svg");
	for(int k = 0; k < NORDSTEPS; k++)
		createStep(module, k);
	for(int k = 0; k < NORDCARS; k++)
		createCar(module, k);

	createDataEntry(module);

	if(module != NULL)
	{
		module->setWidget(this);
		addChild(module->createDisplay(Vec(mm2px(243.430), yncscape(6.954f, 105.408f))));
		module->cvs.Create(this, 289.747f, 18.542f, Nordschleife::NUM_INPUTS - cvStrip::CVSTRIP_INPUTS, Nordschleife::NUM_PARAMS - cvStrip::CVSTRIP_PARAMS, NORDSTEPS);
	}
	addInput(createInput<PJ301HPort>(Vec(mm2px(187.153), yncscape(112.077f, 8.255f)), module, Nordschleife::RANDOMIZONE));
	addParam(createParam<BefacoPushBig>(Vec(mm2px(202.852f), yncscape(111.705f, 8.999)), module, Nordschleife::M_RESET));
	addInput(createInput<PJ301BPort>(Vec(mm2px(215.507f), yncscape(112.077f, 8.255)), module, Nordschleife::MRESET_IN));
	addParam(createParam<CKD6B>(Vec(mm2px(232.910f), yncscape(1.225f, 6.f)), module, Nordschleife::GANGBANG));
}

void NordschleifeWidget::createDataEntry(Nordschleife *module)
{
	float x = 230.470f; //posizi pulsante centr
	float y = 113.204f;

	addParam(createParam<CKD6B>(Vec(mm2px(x), yncscape(y, 6.f)), module, Nordschleife::DATAENTRY_MODE));
	addParam(createParam<UPSWITCH>(Vec(mm2px(x + 0.686f), yncscape(y + 6.704f, 4.115f)), module, Nordschleife::DATAENTRY_UP));
	addParam(createParam<DNSWITCH>(Vec(mm2px(x + 0.686f), yncscape(y - 4.819f, 4.115f)), module, Nordschleife::DATAENTRY_DOWN));
	addParam(createParam<LEFTSWITCH>(Vec(mm2px(x - 4.819f), yncscape(y + 0.686f, 4.627f)), module, Nordschleife::DATAENTRY_LEFT));
	addParam(createParam<RIGHTSWITCH>(Vec(mm2px(x + 6.704f), yncscape(y + 0.686f, 4.627f)), module, Nordschleife::DATAENTRY_RIGHT));
}

void NordschleifeWidget::createCar(Nordschleife *module, int index)
{
	float x = 185.222f;
	float y = 92.026f;
	float y_inc = -10.055f;

	x += 10.639f * index;
	addChild(createLight<TinyLight<WhiteLight>>(Vec(mm2px(x + 8.255), yncscape(y + 7.157f, 1.088f)), module, Nordschleife::LAP_LED + index));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(x), yncscape(y, 8.255f)), module, Nordschleife::CAR_LAP + index));
	y += y_inc;
	addOutput(createOutput<PJ301BPort>(Vec(mm2px(x), yncscape(y, 8.255f)), module, Nordschleife::CAR_POSITION + index));
	y += y_inc;
	addInput(createInput<PJ301YPort>(Vec(mm2px(x), yncscape(y, 8.255f)), module, Nordschleife::CAR_RESET + index));
	y += y_inc;
	addInput(createInput<PJ301RPort>(Vec(mm2px(x), yncscape(y, 8.255f)), module, Nordschleife::CAR_CLOCK + index));
	y += y_inc;
	addChild(createLight<TinyLight<WhiteLight>>(Vec(mm2px(x + 8.255), yncscape(y + 7.157f, 1.088f)), module, Nordschleife::TRIG_LED + index));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(x), yncscape(y, 8.255f)), module, Nordschleife::CAR_TRIG + index));
	y += y_inc;
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(x), yncscape(y, 8.255f)), module, Nordschleife::CAR_AUX + index));
	y += y_inc;
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(x), yncscape(y, 8.255f)), module, Nordschleife::CAR_CV + index));
	y += y_inc;
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(x), yncscape(y, 8.255f)), module, Nordschleife::CAR_GATE + index));

	addParam(createParam<CKD6B>(Vec(mm2px(x + 0.423f), yncscape(1.225f, 6.f)), module, Nordschleife::CAR_SELECT + index));
}

void NordschleifeWidget::createStep(Nordschleife *module, int index)
{
	float x = 6.218;
	float y = 114.95;
	int c = index % 8;
	int r = index / 8;

	x += 22.346 * c;
	y -= 15.409 * r;

	if(c == 0 || c == 4)
		addParam(createParam<Davies1900hFixRedKnob>(Vec(mm2px(x), yncscape(y, 9.525f)), module, Nordschleife::VOLTAGE_1 + index));
	else
		addParam(createParam<Davies1900hFixBlackKnob>(Vec(mm2px(x), yncscape(y, 9.525f)), module, Nordschleife::VOLTAGE_1 + index));
	addParam(createParam<StepSelector>(Vec(mm2px(x), yncscape(y - 2.932f, 2.5f)), module, Nordschleife::STEPSELECT_1 + index));

	addOutput(createOutput<portSmall>(Vec(mm2px(x + 13.222), yncscape(y + 3.365, 5.885f)), module, Nordschleife::OUT_A + index));
	addOutput(createOutput<portBLUSmall>(Vec(mm2px(x + 13.222), yncscape(y - 2.532, 5.885f)), module, Nordschleife::OUT_B + index));

	addChild(createLight<SmallLight<GreenLight>>(Vec(mm2px(x + 10.339f), yncscape(y + 7.35f, 2.176f)), module, Nordschleife::LOTUS_LED + index));
	addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(x + 10.339f), yncscape(y + 4.027f, 2.176f)), module, Nordschleife::BRABHAM_LED + index));
	addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(x + 10.339f), yncscape(y + 0.792f, 2.176f)), module, Nordschleife::FERRARI_LED + index));
	addChild(createLight<SmallLight<PurpleLight>>(Vec(mm2px(x + 10.339f), yncscape(y - 2.532f, 2.176f)), module, Nordschleife::HESKETH_LED + index));
}

Menu *NordschleifeWidget::addContextMenu(Menu *menu)
{
	menu->addChild(new RandomizeItem(module));

	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Randomize Pitch", this, RANDOMIZE_PITCH));
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Randomize Mode", this, RANDOMIZE_MODE));
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Randomize Probability", this, RANDOMIZE_PROBABILITY));
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Randomize Repetitions", this, RANDOMIZE_REPETITIONS));
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Randomize Direction", this, RANDOMIZE_DIRECTION));
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Randomize Path", this, RANDOMIZE_PATH));
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Randomize On Collision", this, RANDOMIZE_ONCRASH));
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Randomize Angle", this, RANDOMIZE_ANGLE));
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Randomize Car Offset", this, RANDOMIZE_OFFSET));
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Randomize Step Offset", this, RANDOMIZE_STEPOFFSET));

	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Pitch Quantization", this, QUANTIZE_PITCH));
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Set all steps to <current>", this, SET_VOLTAGE_TO_CURRENT));
	return menu;
}

void NordschleifeWidget::onMenu(int action)
{
	switch(action)
	{
		case RANDOMIZE_PITCH: std_randomize(Nordschleife::VOLTAGE_1, Nordschleife::VOLTAGE_1 + NORDSTEPS); break;
		case RANDOMIZE_MODE:        ((Nordschleife *)module)->randrandrand(1); break;
		case RANDOMIZE_PROBABILITY:	((Nordschleife *)module)->randrandrand(2); break;
		case RANDOMIZE_REPETITIONS:	((Nordschleife *)module)->randrandrand(3); break;
		case RANDOMIZE_DIRECTION:	((Nordschleife *)module)->randrandrand(4); break;
		case RANDOMIZE_PATH:		((Nordschleife *)module)->randrandrand(5); break;
		case RANDOMIZE_ONCRASH:		((Nordschleife *)module)->randrandrand(6); break;
		case RANDOMIZE_ANGLE:		((Nordschleife *)module)->randrandrand(7); break;
		case RANDOMIZE_OFFSET:		((Nordschleife *)module)->randrandrand(8); break;
		case RANDOMIZE_STEPOFFSET:	((Nordschleife *)module)->randrandrand(9); break;
		case QUANTIZE_PITCH:		((Nordschleife *)module)->QuantizePitch(); break;
		case SET_VOLTAGE_TO_CURRENT:((Nordschleife *)module)->setAllCurrent(); break;
	}
}

void NordschleifeWidget::RandomizeSubItemItem::onAction(const event::Action &e)
{
	md->theRandomizer ^= randomizeDest;
}

NordschleifeWidget::RandomizeSubItemItem::RandomizeSubItemItem(Module *m, const char *title, int action)
{
	md = (Nordschleife *)m;
	text = title;
	randomizeDest = action;
	rightText = CHECKMARK((md->theRandomizer & randomizeDest) != 0);
}

void NordschleifeWidget::onHoverKey(const event::HoverKey &e)
{
	SequencerWidget::onHoverKey(e);
	if(module != NULL && !e.isConsumed() && (e.action == GLFW_PRESS || e.action == GLFW_REPEAT))
	{
		switch(e.key)
		{
			case GLFW_KEY_KP_1:
			case GLFW_KEY_KP_2:
			case GLFW_KEY_KP_3:
			case GLFW_KEY_KP_4:
				e.consume(this);
				((Nordschleife *)module)->setCar(e.key - GLFW_KEY_KP_1);
				break;

			case GLFW_KEY_KP_ENTER:
			case GLFW_KEY_ENTER:
				e.consume(this);
				((Nordschleife *)module)->setKey(GLFW_KEY_ENTER);
				break;

			case GLFW_KEY_KP_0:
			case GLFW_KEY_KP_DIVIDE:
			case GLFW_KEY_KP_MULTIPLY:
			case GLFW_KEY_KP_SUBTRACT:
			case GLFW_KEY_KP_ADD:
				e.consume(this);
				((Nordschleife *)module)->setKey(e.key);
				break;
		}
	}
}