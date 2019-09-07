#include "../include/Nordschleife.hpp"
#include "../include/nordschleifeUI.hpp"

std::vector<std::string> Nordschleife::carNames = {"Lotus", "Brabham", "Ferrari", "Hesketh"};
std::vector<std::string> Nordschleife::pathNames = {"Tiergarten", "Karussell", "Adenauer Forst", "Metzgesfeld","Eiskurve", "Aremberg", "Flugplatz", "Hohe Acht", "Pflanzgarten", "Kallenhard","Steilstrecke","Eschbach"};
int Nordschleife::paths[12][64] =
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
			cars[k].process(deltaTime);
			if(NordschleifeStep::Collision(k))
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
			case GLFW_KEY_KP_SUBTRACT:
				if(--curStep < r * 8) curStep = r * 8 + 7;
				setStep(curStep);
				break;
			case GLFW_KEY_KP_ADD:
				if(++curStep > 7 + r * 8) curStep = r * 8;
				setStep(curStep);
				break;
			case GLFW_KEY_KP_DIVIDE:
				curStep -= 8;
				if(curStep < 0) curStep = 56 + c;
				setStep(curStep);
				break;
			case GLFW_KEY_KP_MULTIPLY:
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

	if(theRandomizer & NordschleifeWidget::RANDOMIZE_LAQUALUNQUE)
	{
		randrandrand(int(random::uniform() * 7));
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
				steps[k].mode = (NordschleifeStep::StepMode)int(random::uniform() * 5);
			break;

		case 2: //probabil
			for(int k = 0; k < NORDSTEPS; k++)
				steps[k].probability = 1+int(random::uniform() * 100);
			break;

		case 3: //reps
			for(int k = 0; k < NORDSTEPS; k++)
				steps[k].repeats = 1+int(random::uniform() *8);
			break;

		case 4: //direc
			for(int k = 0; k < NORDCARS; k++)
				cars[k].direction = (NordschleifeCar::CarDirection)int(random::uniform() * 5);
			break;

		case 5: //path
			for(int k = 0; k < NORDCARS; k++)
				cars[k].path = int(random::uniform() * 12);
			break;

		case 6: //crsh
			for(int k = 0; k < NORDCARS; k++)
				cars[k].collision = (NordschleifeCar::CarCollision)int(random::uniform() * 4);
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
	float scnd_half = -6 + display->box.size.x / 2;

	// cars
	int row = 0;
	nsFields[NordschleifeFields::shlfDirection].set(0, row++, "Direction: ", {"Forward", "Backward", "Alternate", "Brownian", "Random"}, [this] {return cars[selectedCar].direction; }, [this](int i) {cars[selectedCar].direction = (NordschleifeCar::CarDirection)i; });
	nsFields[NordschleifeFields::shlfPath].set(0, row++, "Path: ", Nordschleife::pathNames, [this] {return cars[selectedCar].path; }, [this](int i) {cars[selectedCar].path = i; });
	nsFields[NordschleifeFields::shlfCollision].set(0, row++, "On Crash: ", {"Ignore", "Invert", "90 left", "90 right"}, [this] {return cars[selectedCar].collision; }, [this](int i) {cars[selectedCar].collision = (NordschleifeCar::CarCollision)i; });
	nsFields[NordschleifeFields::shlfFrom].set(0, row, "From: ", 0, 62, [this] {return cars[selectedCar].stepFrom; }, [this](int i) {cars[selectedCar].stepFrom = i; }, 1);
	nsFields[NordschleifeFields::shlfTo].set(scnd_half + 8, row++, "To: ", 1, 63, [this] {return cars[selectedCar].stepTo; }, [this](int i) {cars[selectedCar].stepTo = i; }, 1);
	nsFields[NordschleifeFields::shlfStrtgEvery].set(0, row, "Pit every:", 0, 24, [this] {return cars[selectedCar].strategyEvery; }, [this](int i) {cars[selectedCar].strategyEvery = i; });
	nsFields[NordschleifeFields::shlfStrtgFor].set(scnd_half+15, row, "for: ", 1, 12, [this] {return cars[selectedCar].strategyFor; }, [this](int i) {cars[selectedCar].strategyFor = i; });

	// steps
	nsFields[NordschleifeFields::shlfStep].set(0, row, "Step #", 0, 63, [this] {return selectedStep; }, [this](int i) {setStep(i); }, 1, true);
	row += 4;
	nsFields[NordschleifeFields::shlfMode].set(0, row++, "Mode: ", {"Off", "On", "Skip", "Legato", "Reset"}, [this] {return steps[selectedStep].mode; }, [this](int i) {steps[selectedStep].mode = (NordschleifeStep::StepMode)i; });
	nsFields[NordschleifeFields::shlfProbab].set(0, row++, "Probability: ", 1, 100, [this] {return steps[selectedStep].probability; }, [this](int i) {steps[selectedStep].probability = i; });
	nsFields[NordschleifeFields::shlfRepeats].set(0, row++, "Reps: ", 1, 8, [this] {return steps[selectedStep].repeats; }, [this](int i) {steps[selectedStep].repeats = i; });

	nsFields[NordschleifeFields::shlfOutA].set(0, row++, "Out A: ", Nordschleife::carNames, [this] {return steps[selectedStep].outA; }, [this](int i) {steps[selectedStep].outA = i; });
	nsFields[NordschleifeFields::shlfOutB].set(0, row++, "Out B: ", Nordschleife::carNames, [this] {return steps[selectedStep].outB; }, [this](int i) {steps[selectedStep].outB = i; });
}

void Nordschleife::init_tables()
{
	for (int n = 0; n < NORDSTEPS; n++)
	{
		int r=n/8;
		int c=n%8;
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
	CREATE_PANEL(module, this, 50, "res/modules/Nordschleife.svg");
	for(int k = 0; k < NORDSTEPS; k++)
		createStep(module, k);
	for(int k = 0; k < NORDCARS; k++)
		createCar(module, k);

	createDataEntry(module);

	if(module != NULL)
	{
		module->setWidget(this);
		addChild(module->createDisplay(Vec(mm2px(185.942), yncscape(82.4f, 41.f))));
		module->cvs.Create(this, 241.917f, 7.165f, Nordschleife::NUM_INPUTS - cvStrip::CVSTRIP_INPUTS, Nordschleife::NUM_PARAMS - cvStrip::CVSTRIP_PARAMS, NORDSTEPS);
	}
	addInput(createInput<PJ301HPort>(Vec(mm2px(231.172), yncscape(83.583f, 8.255f)), module, Nordschleife::RANDOMIZONE));
	addParam(createParam<BefacoPushBig>(Vec(mm2px(243.076f), yncscape(98.923f, 8.999)), module, Nordschleife::M_RESET));
	addInput(createInput<PJ301BPort>(Vec(mm2px(229.726f), yncscape(99.295f, 8.255)), module, Nordschleife::MRESET_IN));
}

void NordschleifeWidget::createDataEntry(Nordschleife *module)
{
	float x = 234.672f;
	float y = 113.052f;

	addParam(createParam<CKD6B>(Vec(mm2px(x), yncscape(y, 6.f)), module, Nordschleife::DATAENTRY_MODE));
	addParam(createParam<UPSWITCH>(Vec(mm2px(x + 0.686f), yncscape(y + 6.704f, 4.115f)), module, Nordschleife::DATAENTRY_UP));
	addParam(createParam<DNSWITCH>(Vec(mm2px(x + 0.686f), yncscape(y - 4.819f, 4.115f)), module, Nordschleife::DATAENTRY_DOWN));
	addParam(createParam<LEFTSWITCH>(Vec(mm2px(x - 4.819f), yncscape(y + 0.686f, 4.627f)), module, Nordschleife::DATAENTRY_LEFT));
	addParam(createParam<RIGHTSWITCH>(Vec(mm2px(x + 6.704f), yncscape(y + 0.686f, 4.627f)), module, Nordschleife::DATAENTRY_RIGHT));
}

void NordschleifeWidget::createCar(Nordschleife *module, int index)
{
	float x = 185.222f;

	x += 10.639f * index;
	addInput(createInput<PJ301YPort>(Vec(mm2px(x), yncscape(52.759f, 8.255f)), module, Nordschleife::CAR_RESET + index));
	addInput(createInput<PJ301RPort>(Vec(mm2px(x), yncscape(41.540f, 8.255f)), module, Nordschleife::CAR_CLOCK + index));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(x), yncscape(30.322f, 8.255f)), module, Nordschleife::CAR_CV + index));
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(x), yncscape(19.103f, 8.255f)), module, Nordschleife::CAR_GATE + index));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(x), yncscape(63.978f, 8.255f)), module, Nordschleife::CAR_LAP + index));
	addParam(createParam<CKD6B>(Vec(mm2px(x + 0.423f), yncscape(1.225f, 6.f)), module, Nordschleife::CAR_SELECT + index));
	addChild(createLight<TinyLight<BlueLight>>(Vec(mm2px(x + 8.255), yncscape(71.145f, 1.088f)), module, Nordschleife::LAP_LED+ index));
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
		addParam(createParam<Davies1900hFixBlackKnob>(Vec(mm2px(x), yncscape(y, 9.525f)), module, Nordschleife::VOLTAGE_1 + index));
	else
		addParam(createParam<Davies1900hFixRedKnob>(Vec(mm2px(x), yncscape(y, 9.525f)), module, Nordschleife::VOLTAGE_1 + index));
	addParam(createParam<StepSelector>(Vec(mm2px(x), yncscape(y - 2.532f, 1.879f)), module, Nordschleife::STEPSELECT_1 + index));

	addOutput(createOutput<portSmall>(Vec(mm2px(x + 13.222), yncscape(y + 3.365, 5.885f)), module, Nordschleife::OUT_A + index));
	addOutput(createOutput<portSmall>(Vec(mm2px(x + 13.222), yncscape(y - 2.532, 5.885f)), module, Nordschleife::OUT_B + index));

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

	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Pitch Quantization", this, QUANTIZE_PITCH));
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
		case QUANTIZE_PITCH: ((Nordschleife *)module)->QuantizePitch(); break;
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