#include "../include/common.hpp"
#include "../include/quattro.hpp"

int quattro::ledStrips[4] = {LEDSTRIP1, LEDSTRIP2, LEDSTRIP3, LEDSTRIP4};

void quattro::process(const ProcessArgs &args)
{
	float deltaTime = 1.0 / args.sampleRate;
	if(masterReset.process(params[M_RESET].value + inputs[MRESET_IN].getVoltage()))
	{
		reset(deltaTime);
	} else
	{
		if(pWidget != NULL && rndTrigger.process(inputs[RANDOMIZONE].getVoltage()))
			randrandrand();

		float rec_smp;
		int rec_step;
		if(cvs.IsRecAvailable(&rec_smp, &rec_step))
		{
			pWidget->SetValue(quattro::VOLTAGE_1 + rec_step, rec_smp);
		}

		int forceStep = -1;
		for(int k = 0; k < QUATTRO_NUM_STEPS && forceStep == -1; k++)
		{
			if(setStepTrig[k].process(inputs[quattro::SETSTEP1 + k].getVoltage()))
			{
				forceStep = k;
				break;
			}
		}

		for(int k = 0; k < NUM_STRIPS; k++)
			strip[k].process(forceStep, deltaTime);
	}
}

void quattro::reset(float deltaTime)
{
	for(int k = 0; k < NUM_STRIPS; k++)
		strip[k].reset(deltaTime);
}

void quattro::on_loaded()
{
	load();
	cvs.Init(pWidget);
}

void quattro::load()
{
	reset(0.f);
}

void quattro::randrandrand()
{
	if(theRandomizer & quattroWidget::RANDOMIZE_PITCH)
		randrandrand(0);

	if(theRandomizer & quattroWidget::RANDOMIZE_MODE)
		randrandrand(1);

	if(theRandomizer & quattroWidget::RANDOMIZE_DIRECTION)
		randrandrand(2);

	if(theRandomizer & quattroWidget::RANDOMIZE_ABCD)
		randrandrand(3);

	if(theRandomizer & quattroWidget::RANDOMIZE_LAQUALUNQUE)
	{
		randrandrand(int(random::uniform() * 4));
	}
}

void quattro::randrandrand(int action)
{
	switch(action)
	{
		case 0:
			pWidget->std_randomize(quattro::VOLTAGE_1, quattro::VOLTAGE_1 + QUATTRO_NUM_STEPS);
			break;

		case 1:
			pWidget->std_randomize(quattro::MODE, quattro::MODE + QUATTRO_NUM_STEPS);
			break;

		case 2:
			pWidget->std_randomize(quattro::DIRECTION1, quattro::DIRECTION1 + NUM_STRIPS);
			break;

		case 3:
			pWidget->std_randomize(quattro::STRIPSEL_1, quattro::STRIPSEL_1 + QUATTRO_NUM_STEPS);
			break;
	}
}

void quattro::QuantizePitch()
{
	for(int k = 0; k < QUATTRO_NUM_STEPS; k++)
		params[VOLTAGE_1 + k].value = pWidget->quantizePitch(VOLTAGE_1 + k, params[VOLTAGE_1 + k].value, cvs);
}

#define INCY(k) (-k * 14.798f)
#define INCX(k) (k * 23.651f)
quattroWidget::quattroWidget(quattro *module) : SequencerWidget()
{
	if(module != NULL)
		module->setWidget(this);

	CREATE_PANEL(module, this, 52, "res/modules/quattro.svg");

	addInput(createInput<PJ301HPort>(Vec(mm2px(231.288f), yncscape(11.951f, 8.255)), module, quattro::RANDOMIZONE));
	if(module != NULL)
		module->cvs.Create(this, 251.717f, 18.530f, quattro::NUM_INPUTS-cvStrip::CVSTRIP_INPUTS, quattro::NUM_PARAMS - cvStrip::CVSTRIP_PARAMS, QUATTRO_NUM_STEPS);

	ParamWidget *pwdg = createParam<BefacoPushBig>(Vec(mm2px(24.889f), yncscape(112.537f, 8.999)), module, quattro::M_RESET);
	addParam(pwdg);
	addInput(createInput<PJ301BPort>(Vec(mm2px(9.994f), yncscape(112.909f, 8.255)), module, quattro::MRESET_IN));

	for(int k = 0; k < NUM_STRIPS; k++)
	{
		addInput(createInput<PJ301RPort>(Vec(mm2px(4.036f), yncscape(76.947f + INCY(k), 8.255)), module, quattro::CLOCK1 + k));
		addInput(createInput<PJ301BPort>(Vec(mm2px(15.07f), yncscape(76.947f + INCY(k), 8.255)), module, quattro::DIRECTION_IN1 + k));
		addParam(createParam<CKSSThreeFix>(Vec(mm2px(24.201f), yncscape(76.074f + INCY(k), 10.000f)), module, quattro::DIRECTION1 + k));
		addInput(createInput<PJ301YPort>(Vec(mm2px(30.867f), yncscape(76.947f + INCY(k), 8.255)), module, quattro::RESET1 + k));

		addOutput(createOutput<PJ301GPort>(Vec(mm2px(223.548f), yncscape(76.947f + INCY(k), 8.255)), module, quattro::CV1 + k));
		addOutput(createOutput<PJ301WPort>(Vec(mm2px(239.028f), yncscape(76.947f + INCY(k), 8.255)), module, quattro::GATE1 + k));
	}

	for(int k = 0; k < QUATTRO_NUM_STEPS; k++)
		create_strip(module, k);
}

void quattroWidget::create_strip(quattro *module, int n)
{
	int xleft = 39.117f + INCX(n);

	ParamWidget *pwdg = createParam<Davies1900hLargeFixRedKnob>(Vec(mm2px(xleft + 2.012f - 2.04f), yncscape(109.190f + 3.49f, 11.430)), module, quattro::VOLTAGE_1 + n);
	addParam(pwdg);
	pwdg = createParam<Davies1900hFixBlackKnobSmall>(Vec(mm2px(xleft + 3.726f), yncscape(93.989f, 8.0)), module, quattro::MODE + n);
	((Davies1900hFixBlackKnobSmall *)pwdg)->snap = true;
	addParam(pwdg);

	for(int k = 0; k < NUM_STRIPS; k++)
	{
		ModuleLightWidget *plight;
		if(k % 2)
			plight = createLight<LargeLight<WhiteLight>>(Vec(mm2px(xleft + 5.137f), yncscape(78.485f + INCY(k), 5.179f)), module, quattro::ledStrips[k] + n);
		else
			plight = createLight<LargeLight<RedLight>>(Vec(mm2px(xleft + 5.137f), yncscape(78.485f + INCY(k), 5.179f)), module, quattro::ledStrips[k] + n);
		addChild(plight);
	}

	addInput(createInput<PJ301BPort>(Vec(mm2px(xleft), yncscape(20.228f, 8.255f)), module, quattro::SETSTEP1 + n));
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(xleft + 7.726f), yncscape(11.761f, 8.255f)), module, quattro::CURSTEP1 + n));

	pwdg = createParam<ABCDSwitch>(Vec(mm2px(xleft + 2.469f), yncscape(2.775, 7.093)), module, quattro::STRIPSEL_1 + n);
	addParam(pwdg);
}


Menu *quattroWidget::addContextMenu(Menu *menu)
{
	menu->addChild(new RandomizeItem(module));

	menu->addChild(new SeqMenuItem<quattroWidget>("Randomize Pitch", this, RANDOMIZE_PITCH));
	menu->addChild(new SeqMenuItem<quattroWidget>("Randomize Modes", this, RANDOMIZE_MODE));
	menu->addChild(new SeqMenuItem<quattroWidget>("Randomize Direction", this, RANDOMIZE_DIRECTION));
	menu->addChild(new SeqMenuItem<quattroWidget>("Randomize ABCD", this, RANDOMIZE_ABCD));
	menu->addChild(new SeqMenuItem<quattroWidget>("Pitch Quantization", this, QUANTIZE_PITCH));
	return menu;
}

void quattroWidget::onMenu(int action)
{
	switch(action)
	{
		case RANDOMIZE_PITCH: std_randomize(quattro::VOLTAGE_1, quattro::VOLTAGE_1 + QUATTRO_NUM_STEPS); break;
		case RANDOMIZE_MODE: std_randomize(quattro::MODE, quattro::MODE + QUATTRO_NUM_STEPS); break;
		case RANDOMIZE_DIRECTION: std_randomize(quattro::DIRECTION1, quattro::DIRECTION1 + NUM_STRIPS); break;
		case RANDOMIZE_ABCD: std_randomize(quattro::STRIPSEL_1, quattro::STRIPSEL_1 + QUATTRO_NUM_STEPS); break;
		case QUANTIZE_PITCH: ((quattro *)module)->QuantizePitch(); break;
	}
}

quattroWidget::RandomizeSubItemItem::RandomizeSubItemItem(Module *m, const char *title, int action)
{
	md = (quattro *)m;
	text = title;
	randomizeDest = action;
	rightText = CHECKMARK((md->theRandomizer & randomizeDest) != 0);
}

void quattroWidget::RandomizeSubItemItem::onAction(const event::Action &e)
{
	md->theRandomizer ^= randomizeDest;
}

void quattroStrip::Init(quattro *pmodule, int n)
{
	stripID = n;
	pModule = pmodule;
	resetPulseGuard.reset();
	curStep = 0;
	moving_bwd = false;
	beginPulse();
}

void quattroStrip::process(int forceStep, float deltaTime)
{
	if(pModule != NULL)
	{
		if(resetTrig.process(pModule->inputs[quattro::RESET1 + stripID].getVoltage()))
			reset(deltaTime);
		else
		{
			int pulseStatus = resetPulseGuard.process(deltaTime);
			if(pulseStatus == 0) //gioco regolare, nessun reset pending
			{
				if(forceStep >= 0)
					prenotazioneDiChiamata = forceStep;

				int clk = clockTrigger.process(pModule->inputs[quattro::CLOCK1 + stripID].getVoltage()); // 1=rise, -1=fall
				if(clk == 1)
				{
					move_next();
					bool silent;
					switch(getStepMode())
					{
						default:
							silent = false;
							break;

						case SKIP:
							silent = true;
							break;
					}
					beginPulse(silent);
				} else if(clk == -1)
				{
					endPulse(getStepMode() == SLIDE);
				} 

			} else if(pulseStatus == -1)
				endPulse(false);
		}
	}
}

int quattroStrip::getDirection()
{
	return (int)roundf(getModulableParam(pModule, quattro::DIRECTION1 + stripID, quattro::DIRECTION_IN1 + stripID, 0, 2));
}

void quattroStrip::reset_curstep(int movement)
{
	switch(movement)
	{
		case 0:  // fwd
			curStep = 0;
			break;

		case 1: // bwd
			curStep = QUATTRO_NUM_STEPS - 1;
			break;

		case 2: // el pendulun
			curStep = moving_bwd ? QUATTRO_NUM_STEPS - 1 : 0;
			break;
	}
}

void quattroStrip::move_next()
{
	if(prenotazioneDiChiamata >= 0)
	{
		curStep = prenotazioneDiChiamata;
		prenotazioneDiChiamata = -1;
		return;
	}
	int movement = getDirection();
	
	if(getStepMode() == RESET)
	{
		reset_curstep(movement);
		switch(movement)
		{
			case 0:  // fwd
				curStep = 0;
				break;

			case 1: // bwd
				curStep = QUATTRO_NUM_STEPS - 1;
				break;

			case 2: // el pendulun
				curStep = moving_bwd ? QUATTRO_NUM_STEPS - 1 : 0;
				break;
		}
		return;
	}

	for(int k = 0; k < QUATTRO_NUM_STEPS; k++)
	{
		switch(movement)
		{
			case 0:  // fwd
			{
				if(++curStep >= QUATTRO_NUM_STEPS)
					curStep = 0;
			}
			break;

			case 1: // bwd
			{
				if(--curStep < 0)
					curStep = QUATTRO_NUM_STEPS - 1;
			}
			break;

			case 2: // el pendulun
			{
				if(moving_bwd)
				{
					if(--curStep < 0) // siamo a battuta verso sx
					{
						curStep = 1;
						moving_bwd = false;
					}
				} else
				{
					if(++curStep >= QUATTRO_NUM_STEPS) // siamo a battuta verso dx
					{
						curStep = QUATTRO_NUM_STEPS - 2;
						moving_bwd = true;
					}
				}
			}
			break;
		}

		STEPMODE mode = getStepMode();
		if(mode == SKIP)
			continue;
		else if(mode == N_75)
		{
			if((int)(random::uniform() * 4) > 2)
				continue;
		} else if(mode == N_50)
		{
			if((int)(random::uniform() * 4) > 1)
				continue;
		} else if(mode == N_25)
		{
			if((int)(random::uniform() * 4) > 0)
				continue;
		}
	
		break;
	}

}

quattroStrip::STEPMODE quattroStrip::getStepMode()
{
	return  (quattroStrip::STEPMODE)round(pModule->params[quattro::MODE + curStep].value);
}

void quattroStrip::beginPulse(bool silent)
{
	int abcd = (int)roundf(pModule->params[quattro::STRIPSEL_1 + curStep].value);
	pModule->outputs[quattro::CV1 + stripID].setVoltage( pModule->cvs.TransposeableValue(pModule->params[quattro::VOLTAGE_1 + curStep].value));
	pModule->outputs[quattro::GATE1 + stripID].setVoltage( silent ? LVL_OFF : LVL_ON);
	if(stripID == abcd)
		pModule->outputs[quattro::CURSTEP1 + curStep].setVoltage( LVL_ON);
	for(int k = 0; k < QUATTRO_NUM_STEPS; k++)
		pModule->lights[quattro::ledStrips[stripID] + k].value = k == curStep ? LED_ON : LED_OFF;
}

void quattroStrip::endPulse(bool slide)
{
	if(!slide)
		pModule->outputs[quattro::GATE1 + stripID].setVoltage( LVL_OFF);
	for(int k = 0; k < QUATTRO_NUM_STEPS; k++)
		pModule->outputs[quattro::CURSTEP1 + k].setVoltage( LVL_OFF);
}

void quattroStrip::reset(float deltaTime)
{
	if(pModule != NULL && resetPulseGuard.process(deltaTime) == 0)
	{
		endPulse(false);
		curStep = 0;
		reset_curstep(getDirection());
		moving_bwd = false;
		prenotazioneDiChiamata = -1;
		resetPulseGuard.trigger(PULSE_TIME);
		beginPulse();
	}
}