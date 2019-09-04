#include "../include/Nordschleife.hpp"
#include "../include/nordschleifeUI.hpp"

void Nordschleife::process(const ProcessArgs &args)
{
	if(pWidget != NULL && rndTrigger.process(inputs[RANDOMIZONE].value))
		randrandrand();

	car_select();
	if(lazyCheck++ % 20 == 0)
		step_select();
	data_entry();
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

	if(params[DATAENTRY_MODE].getValue() == 0)  //move by step?
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
				if(++curStep > 7+r * 8) curStep = r * 8;
				setStep(curStep);
				break;
			case GLFW_KEY_KP_SUBTRACT:
				curStep -= 8;
				if(curStep< 0) curStep = 56+c;
				setStep(curStep);
				break;
			case GLFW_KEY_KP_ADD:
				curStep += 8;
				if(curStep > 63) curStep = c;
				setStep(curStep);
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

	if(theRandomizer & NordschleifeWidget::RANDOMIZE_LAQUALUNQUE)
	{
		randrandrand(int(random::uniform() * 1));
	}
}

void Nordschleife::randrandrand(int action)
{
	switch(action)
	{
		case 0:
			pWidget->std_randomize(Nordschleife::VOLTAGE_1, Nordschleife::VOLTAGE_1 + NORDSTEPS);
			break;
	}
}

void Nordschleife::QuantizePitch()
{
	for(int k = 0; k < NORDSTEPS; k++)
		params[VOLTAGE_1 + k].value = pWidget->quantizePitch(VOLTAGE_1 + k, params[VOLTAGE_1 + k].value, cvs);
}

NordschleifeWidget::NordschleifeWidget(Nordschleife *module)
{
	CREATE_PANEL(module, this, 49, "res/modules/Nordschleife.svg");
	for(int k = 0; k < NORDSTEPS; k++)
		createStep(module, k);
	for(int k = 0; k < NORDCARS; k++)
		createCar(module, k);

	createDataEntry(module);

	if(module != NULL)
	{
		module->setWidget(this);
		nordDisplay *display = createWidget<nordDisplay>(Vec(mm2px(185.942), yncscape(82.4f, 41.f)));
		display->setModule(module);
		addChild(display);

		module->cvs.Create(this, 236.228f, 15.367f, Nordschleife::NUM_INPUTS - cvStrip::CVSTRIP_INPUTS, Nordschleife::NUM_PARAMS - cvStrip::CVSTRIP_PARAMS, NORDSTEPS);
	}
	addInput(createInput<PJ301HPort>(Vec(mm2px(236.274), yncscape(3.936f, 8.255f)), module, Nordschleife::RANDOMIZONE));

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
	menu->addChild(new SeqMenuItem<NordschleifeWidget>("Pitch Quantization", this, QUANTIZE_PITCH));
	return menu;
}

void NordschleifeWidget::onMenu(int action)
{
	switch(action)
	{
		case RANDOMIZE_PITCH: std_randomize(Nordschleife::VOLTAGE_1, Nordschleife::VOLTAGE_1 + NORDSTEPS); break;
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
	if(module != NULL && !e.isConsumed() && e.action == GLFW_PRESS)
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