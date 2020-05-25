#include "../include/Klee.hpp"

uint8_t Klee::bitfield[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

void Klee::on_loaded()
{
	load();
}

void Klee::randrandrand()
{
	if(theRandomizer & KleeWidget::RANDOMIZE_PITCH)
		randrandrand(0);

	if(theRandomizer & KleeWidget::RANDOMIZE_BUS)
		randrandrand(1);

	if(theRandomizer & KleeWidget::RANDOMIZE_LOAD)
		randrandrand(2);

	if(theRandomizer & KleeWidget::RANDOMIZE_LAQUALUNQUE)
	{
		randrandrand(int(random::uniform() * 3));
	}
}

void Klee::randrandrand(int action)
{
	switch(action)
	{
		case 0:
			pWidget->std_randomize(Klee::PITCH_KNOB, Klee::PITCH_KNOB + 16);
			break;

		case 1:
			pWidget->std_randomize(Klee::GROUPBUS, Klee::GROUPBUS + 16); ;
			break;

		case 2:
			pWidget->std_randomize(Klee::LOAD_BUS, Klee::LOAD_BUS + 16);
			break;
	}
}

void Klee::process(const ProcessArgs &args)
{
	float deltaTime = 1.0 / args.sampleRate;

	if(loadTrigger.process(params[LOAD_PARAM].value + inputs[LOAD_INPUT].value))
	{
		load();
	} else if(rndTrigger.process(inputs[RANDOMIZONE].value))
	{
		if(pWidget != NULL)
			randrandrand();
	}

	int clk = clockTrigger.process(inputs[EXT_CLOCK_INPUT].value + params[STEP_PARAM].value); // 1=rise, -1=fall
	if(clk == 1)
	{
		sr_rotate();
		update_bus();
		populate_outputs();
	}

	if(clk != 0)
	{
		populate_gate(clk);
	}

	check_triggers(deltaTime);

	showValues();

}

void Klee::load()
{
	for(int k = 0; k < 16; k++)
	{
		shiftRegister.P[k] = isSwitchOn(this, LOAD_BUS + k);
	}
}


void Klee::update_bus()
{
	bool bus1 = bus_active[0];
	for(int k = 0; k < 3; k++)
		bus_active[k] = false;

	for(int k = 0; k < 16; k++)
	{
		if(shiftRegister.P[k])
		{
			bus_active[getValue3(k)] = true;
		}
	}
	if(isSwitchOn(this, BUS2_MODE))
		bus_active[1] = bus_active[0] && bus_active[2];
	else
		bus_active[1] &= !(bus_active[0] || bus_active[2]);  //BUS 2: NOR 0 , 3

	//bus1 load
	if(isSwitchOn(this, BUS1_LOAD) && !bus1 && bus_active[0])
		load();
}

int Klee::getValue3(int k)
{
	int v = roundf(params[GROUPBUS + k].value);
	return 2 - v;
}

void Klee::check_triggers(float deltaTime)
{
	for(int k = 0; k < 3; k++)
	{
		if(outputs[TRIG_OUT + k].value > 0.5 && !triggers[k].process(deltaTime))
		{
			outputs[TRIG_OUT + k].value = LVL_OFF;
		}
	}
}

void Klee::populate_gate(int clk)
{
	for(int k = 0; k < 3; k++)
	{
		// gate
		if(clk == 1)  // rise
		{
			outputs[GATE_OUT + k].value = bus_active[k] ? LVL_ON : LVL_OFF;
		} else // fall
		{
			if(!bus_active[k] || !isSwitchOn(this, BUS_MERGE + k))
				outputs[GATE_OUT + k].value = LVL_OFF;
		}
	}
}

void Klee::populate_outputs()
{
	for(int k = 0; k < 3; k++)
	{
		if(bus_active[k])
		{
			outputs[TRIG_OUT + k].value = LVL_ON;
			triggers[k].trigger(PULSE_TIME);
		}
	}

	float a = 0, b = 0;

	float expander_out = 0;
	uint8_t *p = (uint8_t *)&expander_out;
	*(p+3) = EXPPORT_KLEE;
	for(int k = 0; k < 8; k++)
	{
		if(shiftRegister.A[k])
		{
			a += cvs.TransposeableValue(params[PITCH_KNOB + k].value);
			*(p+1) |= bitfield[k];
		}

		if(shiftRegister.B[k])
		{
			b += cvs.TransposeableValue(params[PITCH_KNOB + k + 8].value);
			*(p+2) |= bitfield[k];
		}
	}

	outputs[EXPANDER_OUT].setVoltage(expander_out);
	outputs[CV_A].value = clamp(a, LVL_MIN, LVL_MAX);
	outputs[CV_B].value = clamp(b, LVL_MIN, LVL_MAX);
	outputs[CV_AB].value = clamp(a + b, LVL_MIN, LVL_MAX);
	outputs[CV_A__B].value = clamp(a - b, LVL_MIN, LVL_MAX);
}

void Klee::showValues()
{
	for(int k = 0; k < 16; k++)
	{
		lights[LED_PITCH + k].value = shiftRegister.P[k] ? LED_ON : LED_OFF;
	}

	for(int k = 0; k < 3; k++)
	{
		lights[LED_BUS + k].value = outputs[GATE_OUT + k].value;
	}
}

void Klee::sr_rotate()
{
	if(!isSwitchOn(this, X28_X16))  // mode 1 x 16
	{
		int fl = shiftRegister.P[15];
		for(int k = 15; k > 0; k--)
		{
			shiftRegister.P[k] = shiftRegister.P[k - 1];
		}
		if(isSwitchOn(this, RND_PAT))
			shiftRegister.P[0] = chance();
		else
			shiftRegister.P[0] = fl;
	} else
	{
		int fla = shiftRegister.A[7];
		int flb = shiftRegister.B[7];
		for(int k = 7; k > 0; k--)
		{
			shiftRegister.A[k] = shiftRegister.A[k - 1];
			shiftRegister.B[k] = shiftRegister.B[k - 1];
		}
		if(isSwitchOn(this, RND_PAT))
			shiftRegister.A[0] = chance();
		else
			shiftRegister.A[0] = fla;
		shiftRegister.B[0] = isSwitchOn(this, B_INV) ? !flb : flb;
	}
}

void Klee::QuantizePitch()
{
	for(int k = 0; k < 16; k++)
		params[PITCH_KNOB + k].value = pWidget->quantizePitch(PITCH_KNOB + k, params[PITCH_KNOB + k].value, cvs);
}

bool Klee::chance()
{	
	return rand() <= getModulableParam(this, RND_THRESHOLD, RND_THRES_IN, 0, LVL_MAX) * RAND_MAX;
}

KleeWidget::KleeWidget(Klee *module) : SequencerWidget()
{
	if(module != NULL)
		module->setWidget(this);

	float nkk_offs = 2.3;
	
	CREATE_PANEL(module, this, 50, "res/modules/KleeModule.svg");

	const float switch_dstx = 22.203 - 11.229;
	for(int k = 0; k < 8; k++)
	{
		// Load switches
		ParamWidget *pwdg = createParam<NKK1>(Vec(mm2px(k * switch_dstx + 11.229), yncscape(114.071 + nkk_offs, 7.336)), module, Klee::LOAD_BUS + k);
		addParam(pwdg);
		
		pwdg = createParam<NKK1>(Vec(mm2px(k * switch_dstx + 118.914), yncscape(114.071 + nkk_offs, 7.336)), module, Klee::LOAD_BUS + k + 8);
		addParam(pwdg);
		
		// BUS switches
		pwdg = createParam<NKK2>(Vec(mm2px(k * switch_dstx + 11.229), yncscape(4.502 + nkk_offs, 7.336)), module, Klee::GROUPBUS + k);
		addParam(pwdg);
	
		pwdg = createParam<NKK2>(Vec(mm2px(k * switch_dstx + 118.914), yncscape(4.502 + nkk_offs, 7.336)), module, Klee::GROUPBUS + k + 8);
		addParam(pwdg);
	}

	// trig/gate out
	float dist_v = 60.582 - 76.986;
	for(int k = 0; k < 3; k++)
	{
		ParamWidget *pwdg = createParam<NKK1>(Vec(mm2px(184.360), yncscape(76.986 + nkk_offs + k * dist_v, 7.336)), module, Klee::BUS_MERGE + k);
		addParam(pwdg);
		
		ModuleLightWidget *plight = createLight<LargeLight<BlueLight>>(Vec(mm2px(205.425), yncscape(78.065 + k * dist_v, 5.179)), module, Klee::LED_BUS + k);
		addChild(plight);

		addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(213.360), yncscape(76.986 + k * dist_v, 8.255)), module, Klee::TRIG_OUT + k));
		addOutput(createOutput<PJ301WPort>(Vec(mm2px(230.822), yncscape(76.986 + k * dist_v, 8.255)), module, Klee::GATE_OUT + k));
	}
	ParamWidget *pwdg = createParam<TL1105Sw>(Vec(mm2px(171.725), yncscape(60.947, 6.607)), module, Klee::BUS2_MODE);
	addParam(pwdg);
	

	//load
	pwdg = createParam<BefacoPushBig>(Vec(mm2px(25.360), yncscape(76.686, 8.999)), module, Klee::LOAD_PARAM);
	addParam(pwdg);

	addInput(createInput<PJ301BPort>(Vec(mm2px(9.218), yncscape(77.058, 8.255)), module, Klee::LOAD_INPUT));

	pwdg = createParam<NKK1>(Vec(mm2px(25.627), yncscape(91.395 + nkk_offs, 7.336)), module, Klee::BUS1_LOAD);
	addParam(pwdg);

	//step
	pwdg = createParam<BefacoPushBig>(Vec(mm2px(25.360), yncscape(24.737, 8.999)), module, Klee::STEP_PARAM);
	addParam(pwdg);

	addInput(createInput<PJ301RPort>(Vec(mm2px(9.218), yncscape(25.109, 8.255)), module, Klee::EXT_CLOCK_INPUT));

	// CV Out
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(213.360), yncscape(113.612, 8.255)), module, Klee::CV_A));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(230.822), yncscape(113.612, 8.255)), module, Klee::CV_B));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(213.360), yncscape(97.207, 8.255)), module, Klee::CV_A__B));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(230.822), yncscape(97.207, 8.255)), module, Klee::CV_AB));

	addOutput(createOutput<PJ301EXP>(Vec(mm2px(230.822), yncscape(25.109, 8.255)), module, Klee::EXPANDER_OUT));

	// mode
	pwdg = createParam<NKK1>(Vec(mm2px(68.915), yncscape(60.582 + nkk_offs, 7.336)), module, Klee::X28_X16);
	addParam(pwdg);     // 2x8 1x16

	pwdg = createParam<NKK1>(Vec(mm2px(97.459), yncscape(60.582 + nkk_offs, 7.336)), module, Klee::RND_PAT);
	addParam(pwdg);     // rnd/pattern

	pwdg = createParam<NKK1>(Vec(mm2px(126.004), yncscape(60.582 + nkk_offs, 7.336)), module, Klee::B_INV);
	addParam(pwdg);     // norm /B inverted
	
	addInput(createInput<PJ301HPort>(Vec(mm2px(184.466), yncscape(25.109, 8.255)), module, Klee::RANDOMIZONE));

	// RND Threshold
	pwdg = createParam<Davies1900hFixBlackKnob>(Vec(mm2px(212.725), yncscape(9.228, 9.525)), module, Klee::RND_THRESHOLD);
	addParam(pwdg);     // rnd threshold
	addInput(createInput<PJ301BPort>(Vec(mm2px(230.822), yncscape(9.863, 8.255)), module, Klee::RND_THRES_IN));

	if(module != NULL)
		module->cvs.Create(this, 241.724f, 41.284f, Klee::NUM_INPUTS - cvMiniStrip::CVMINISTRIP_INPUTS, Klee::NUM_PARAMS - cvMiniStrip::CVMINISTRIP_PARAMS);

	// pitch Knobs + leds
	float pot_x[8] = {39.440, 45.104, 60.976, 83.912, 109.368, 132.304, 148.175, 153.840};
	float led_x[8] = {51.727, 56.481, 69.800, 89.046, 110.408, 129.655, 142.974, 147.727};
	float pot_y_sup[8] = {66.558, 79.131, 89.222, 94.823, 94.823, 89.222, 79.131, 66.558};
	float pot_y_inf[8] = {52.426, 39.814, 29.751, 24.152, 24.152, 29.751, 39.814, 52.426};
	float led_y_sup[8] = {67.079, 76.624, 84.279, 88.527, 88.527, 84.279, 76.624, 67.079};
	float led_y_inf[8] = {58.245, 48.700, 41.045, 36.797, 36.797, 41.045, 48.700, 58.245};
	for(int k = 0; k < 8; k++)
	{
		pwdg = createParam<Davies1900hFixRedKnob>(Vec(mm2px(pot_x[k]), yncscape(pot_y_sup[k], 9.525)), module, Klee::PITCH_KNOB + k);
		addParam(pwdg);

		ModuleLightWidget *plight = createLight<MediumLight<RedLight>>(Vec(mm2px(led_x[k]), yncscape(led_y_sup[k], 3.176)), module, Klee::LED_PITCH + k);
		addChild(plight);
	
		pwdg = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(pot_x[7 - k]), yncscape(pot_y_inf[k], 9.525)), module, Klee::PITCH_KNOB + 8 + k);
		addParam(pwdg);

		plight = createLight<MediumLight<WhiteLight>>(Vec(mm2px(led_x[7 - k]), yncscape(led_y_inf[k], 3.176)), module, Klee::LED_PITCH + k + 8);
		addChild(plight);
	}

}

Menu *KleeWidget::addContextMenu(Menu *menu)
{
	menu->addChild(new RandomizeItem(module));

	menu->addChild(new SeqMenuItem<KleeWidget>("Range -> 1V", this, SET_RANGE_1V));
	menu->addChild(new SeqMenuItem<KleeWidget>("Randomize Pitch", this, RANDOMIZE_PITCH));
	menu->addChild(new SeqMenuItem<KleeWidget>("Randomize Bus", this, RANDOMIZE_BUS));
	menu->addChild(new SeqMenuItem<KleeWidget>("Randomize Load", this, RANDOMIZE_LOAD));
	menu->addChild(new SeqMenuItem<KleeWidget>("Pitch Quantization", this, QUANTIZE_PITCH));
	return menu;
}

void KleeWidget::onMenu(int action)
{
	switch(action)
	{
		case RANDOMIZE_BUS: std_randomize(Klee::GROUPBUS, Klee::GROUPBUS + 16); break;
		case RANDOMIZE_PITCH: std_randomize(Klee::PITCH_KNOB, Klee::PITCH_KNOB + 16); break;
		case RANDOMIZE_LOAD: std_randomize(Klee::LOAD_BUS, Klee::LOAD_BUS + 16); break;
		case SET_RANGE_1V:
		{
			int index = getParamIndex(Klee::RANGE);
			if(index >= 0)
				params[index]->paramQuantity->setValue(0.0);
			index = getParamIndex(Klee::RANGE+1);
			if(index >= 0)
				params[index]->paramQuantity->setValue(1.0);
		}
		break;
		case QUANTIZE_PITCH: ((Klee *)module)->QuantizePitch(); break;
	}
}

KleeWidget::RandomizeSubItemItem::RandomizeSubItemItem(Module *k, const char *title, int action)
{
	kl = (Klee *)k;
	text = title;
	randomizeDest = action;
	rightText = CHECKMARK((kl->theRandomizer & randomizeDest) != 0);
}

void KleeWidget::RandomizeSubItemItem::onAction(const event::Action &e)
{
	kl->theRandomizer ^= randomizeDest;
}
