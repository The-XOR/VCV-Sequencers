#include "../include/common.hpp"
#include "../include/nag.hpp"
#include "../include/nagDisplays.hpp"

void nag::on_loaded()
{
#ifdef DIGITAL_EXT
	connected = 0;
#endif
	load();
}

void nag::load()
{
	reset();
}

void nag::reset()
{
	for (int k = 0; k < NUM_NAGS; k++)
	{
		sequencer[k].reset();
		outputs[OUT_1 + k].value = LVL_OFF;
	}
	theCounter = 0;
	counterRemaining = 0;
}

void nag::updateNags(float dt)
{
	for (int k = 0; k < NUM_NAGS; k++)
	{
		sequencer[k].enabled = params[ENABLE_1 + k].value > 0.5;
		lights[ON_1 + k].value = sequencer[k].Highlight(dt) ? LED_ON : LED_OFF;
		sequencer[k].set(getInput(k, INVERTEX_1, VERTEX_1, MIN_VERTICES, MAX_VERTICES));
		sequencer[k].rotate(getInput(k, INROTATE_1, ROTATE_1, MIN_ROTATE, MAX_ROTATE));
		sequencer[k].skew(getInput(k, INSKEW_1, SKEW_1, MIN_SKEW, MAX_SKEW));
	}
}

int nag::degPerClock() { return  getInput(0, DEGXCLK_IN, DEGXCLK, MIN_DEGXCLOCK, MAX_DEGXCLOCK); }
void nag::process(const ProcessArgs &args)
{
	if (resetTrig.process(inputs[RESET].value) || masterReset.process(params[M_RESET].value))
	{
		reset();
	} else
	{
		if (pWidget != NULL)
		{
			if (rndTrigger.process(inputs[RANDOMIZONE].value))
				randrandrand();
		}
		int degclk = degPerClock();
		bool dm = degclk > 1 && params[DEGMODE].value > 0.1;

		float deltaTime = 1.0 / args.sampleRate;
		updateNags(deltaTime);

		int clk = clockTrig.process(inputs[CLOCK].value); // 1=rise, -1=fall
		if (clk == 1)
		{
			counterRemaining = degclk;
		}
		sclocca(dm, deltaTime);
	}
#ifdef DIGITAL_EXT
	bool dig_connected = false;

#if defined(OSCTEST_MODULE)
	if (oscDrv->Connected())
		dig_connected = true;
	oscDrv->ProcessOSC();
#endif	
	connected = dig_connected ? 1.0 : 0.0;
#endif
}

void nag::sclocca(bool dm, float dt)
{	
	if(counterRemaining > 0)
	{
		if (!dm)
		{
			counterRemaining--;
			theCounter = (theCounter + 1) % NUM_STEPS;
		}
		else
		{
			theCounter = (theCounter + counterRemaining) % NUM_STEPS;
			counterRemaining = 0;
		}
		for (int k = 0; k < NUM_NAGS; k++)
		{
			outputs[OUT_1 + k].value = sequencer[k].bang(theCounter, dt) ? LVL_ON : LVL_OFF;
		}
	}
}

int nag::getInput(int index, int input_id, int knob_id, float mi, float ma)
{
	return (int)roundf(getModulableParam(this, knob_id + index, input_id + index, mi, ma));
}

nagWidget::nagWidget(nag *module) : SequencerWidget()
{
	if (module != NULL)
		module->setWidget(this);
#ifdef OSCTEST_MODULE
	char name[60];
#endif

	CREATE_PANEL(module, this, 45, "res/modules/nag.svg");

	for (int index = 0; index < NUM_NAGS; index++)
	{
		float delta_y = 19.122 * index;
		ParamWidget *pwdg = createParam<NKK1>(Vec(mm2px(80.628), yncscape(110.388 - delta_y, 7.336)), module, nag::ENABLE_1 + index);
		addParam(pwdg);
		#ifdef OSCTEST_MODULE
		if (module != NULL)
		{
			sprintf(name, "/Enable%i", index + 1);
			module->oscDrv->Add(new oscControl(name), pwdg);
		}
		#endif

		pwdg = createParam<Davies1900hFixRedKnobSmall>(Vec(mm2px(105.824), yncscape(108.056 - delta_y, 8.0)), module, nag::VERTEX_1 + index);
		((Davies1900hFixRedKnobSmall *)pwdg)->snap = true;
		addParam(pwdg);
		#ifdef OSCTEST_MODULE
		if (module != NULL)
		{
			sprintf(name, "/Vertices%i", index + 1);
			module->oscDrv->Add(new oscControl(name), pwdg);
		}
		#endif
		addInput(createInput<PJ301GRPort>(Vec(mm2px(118.218), yncscape(108.2 - delta_y, 8.255)), module, nag::INVERTEX_1 + index));

		pwdg = createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(139.902), yncscape(108.056 - delta_y, 8.0)), module, nag::ROTATE_1 + index);
		((Davies1900hFixRedKnobSmall *)pwdg)->snap = true;
		addParam(pwdg);
		
		#ifdef OSCTEST_MODULE
		if (module != NULL)
		{
			sprintf(name, "/Rotate%i", index + 1);
			module->oscDrv->Add(new oscControl(name), pwdg);
		}
		#endif
		addInput(createInput<PJ301GRPort>(Vec(mm2px(152.296), yncscape(108.2 - delta_y, 8.255)), module, nag::INROTATE_1 + index));

		pwdg = createParam<Davies1900hFixBlackKnobSmall>(Vec(mm2px(173.604), yncscape(108.056 - delta_y, 8.0)), module, nag::SKEW_1 + index);
		((Davies1900hFixRedKnobSmall *)pwdg)->snap = true;
		addParam(pwdg);
		#ifdef OSCTEST_MODULE
		if (module != NULL)
		{
			sprintf(name, "/Skew%i", index + 1);
			module->oscDrv->Add(new oscControl(name), pwdg);
		}
		#endif
		addInput(createInput<PJ301GRPort>(Vec(mm2px(185.998), yncscape(108.2-delta_y, 8.255)), module, nag::INSKEW_1 + index));

		addOutput(createOutput<PJ301WPort>(Vec(mm2px(208.211), yncscape(107.928-delta_y, 8.255)), module, nag::OUT_1 + index));
		addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(219.375), yncscape(115.230 - delta_y, 2.176)), module, nag::ON_1 + index));
		addChild(new nag7Segm(module != NULL ? &module->sequencer[index] : NULL, 92.499, 108.231 - delta_y));
	}
	
	ParamWidget *pwdg = createParam<Davies1900hFixRedKnobSmall>(Vec(mm2px(40.702), yncscape(12.631, 8.0)), module, nag::DEGXCLK);
	((Davies1900hFixRedKnobSmall *)pwdg)->snap = true;
	addParam(pwdg);
	addChild(new nag7Segm(module, 27.159f, 12.807f));
	#ifdef OSCTEST_MODULE
	if (module != NULL)
	{
		module->oscDrv->Add(new oscControl("ClockDeg"), pwdg);
	}
	#endif

	addInput(createInput<PJ301YPort>( Vec(mm2px(55.235), yncscape(37.2818, 8.255)), module, nag::RESET));
	addInput(createInput<PJ301RPort>( Vec(mm2px(55.235), yncscape(24.892, 8.255)), module, nag::CLOCK));
	addInput(createInput<PJ301GRPort>(Vec(mm2px(55.235), yncscape(12.503, 8.255)), module, nag::DEGXCLK_IN));

	addInput(createInput<PJ301HPort>(Vec(mm2px(14.195), yncscape(37.281, 8.255)), module, nag::RANDOMIZONE));
	addParam(createParam<TL1105HSw>(Vec(mm2px(56.059), yncscape(4.973, 4.477)), module, nag::DEGMODE));
	addChild(createParam<BefacoPushBig>(Vec(mm2px(40.202), yncscape(36.909, 8.999)), module, nag::M_RESET));

	if (module != NULL)
	{
		nagDisplay *display = createWidget<nagDisplay>(mm2px(Vec(8, 8)));
		display->box.size = mm2px(Vec(60, 60));
		display->SetModule(module);
		addChild(display);
	}

#ifdef DIGITAL_EXT
	if (module != NULL)
		addChild(new DigitalLed(mm2px(7.543), yncscape(8.977, 3.867), &module->connected));
#endif
}

Menu *nagWidget::addContextMenu(Menu *menu)
{
	menu->addChild(new RandomizeItem(module));

	menu->addChild(new SeqMenuItem<nagWidget>("Randomize Off/On", this, RANDOMIZE_OFFON));
	menu->addChild(new SeqMenuItem<nagWidget>("Randomize Numbers", this, RANDOMIZE_VERTICES));
	menu->addChild(new SeqMenuItem<nagWidget>("Randomize Rotation", this, RANDOMIZE_ROTATION));
	menu->addChild(new SeqMenuItem<nagWidget>("Randomize Skew", this, RANDOMIZE_SKEW));

	return menu;
}

void nagWidget::onMenu(int action)
{
	switch (action)
	{
	case RANDOMIZE_OFFON:
		std_randomize(nag::ENABLE_1, nag::ENABLE_1 + NUM_NAGS);
		break;

	case RANDOMIZE_VERTICES:
		std_randomize(nag::VERTEX_1, nag::VERTEX_1 + NUM_NAGS);
		break;

	case RANDOMIZE_ROTATION:
		std_randomize(nag::ROTATE_1, nag::ROTATE_1 + NUM_NAGS);
		break;

	case RANDOMIZE_SKEW:
		std_randomize(nag::SKEW_1, nag::SKEW_1 + NUM_NAGS);
		break;
	}
}

void nag::randrandrand()
{
	if (theRandomizer & nagWidget::RANDOMIZE_OFFON)
		randrandrand(0);

	if (theRandomizer & nagWidget::RANDOMIZE_VERTICES)
		randrandrand(1);

	if (theRandomizer & nagWidget::RANDOMIZE_ROTATION)
		randrandrand(2);

	if (theRandomizer & nagWidget::RANDOMIZE_SKEW)
		randrandrand(3);

	if (theRandomizer & nagWidget::RANDOMIZE_LAQUALUNQUE)
	{
		randrandrand(int(random::uniform() * 4));
	}
}

void nag::randrandrand(int action)
{
	switch (action)
	{
	case 0:
		pWidget->std_randomize(nag::ENABLE_1, nag::ENABLE_1 + NUM_NAGS);
		break;

	case 1:
		pWidget->std_randomize(nag::VERTEX_1, nag::VERTEX_1 + NUM_NAGS);
		break;

	case 2:
		pWidget->std_randomize(nag::ROTATE_1, nag::ROTATE_1 + NUM_NAGS);
		break;

	case 3:
		pWidget->std_randomize(nag::SKEW_1, nag::SKEW_1 + NUM_NAGS);
		break;
	}
}

nagWidget::RandomizeSubItemItem::RandomizeSubItemItem(Module *n, const char *title, int action)
{
	ng = (nag *)n;
	text = title;
	randomizeDest = action;
	rightText = CHECKMARK((ng->theRandomizer & randomizeDest) != 0);
}

void nagWidget::RandomizeSubItemItem::onAction(const event::Action &e)
{
	ng->theRandomizer ^= randomizeDest;
}
