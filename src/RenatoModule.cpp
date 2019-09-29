#include "../include/Renato.hpp"
#include <sstream>


bool Access(Renato *pr, bool is_x, int p) { return is_x ? pr->_accessX(p) : pr->_accessY(p); }

void Renato::led(int l)
{
	for(int r = 0; r < 4; r++)
	{
		for(int c = 0; c < 4; c++)
		{
			int n = c + r * 4;
			lights[LED_1 + n].value = l == n ? LED_ON : LED_OFF;
		}
	}
}

void Renato::setOut(int l, bool on)
{
	for(int r = 0; r < 4; r++)
	{
		for(int c = 0; c < 4; c++)
		{
			int n = c + r * 4;
			lights[LED_1 + n].value = l == n ? LED_ON : LED_OFF;
			outputs[CV_OUTSTEP1 + n].value = (on && l == n) ? LVL_ON : LVL_OFF;
		}
	}
}
void Renato::on_loaded()
{
	#ifdef DIGITAL_EXT
	connected = 0;
	#endif
	load();
	cvs.Init(pWidget);
}

void Renato::load()
{
	seqX.Reset();
	seqY.Reset();
}
void Renato::process(const ProcessArgs &args)
{
	if(resetTrigger.process(inputs[RESET].value) || masterReset.process(params[M_RESET].value))
	{
		seqX.Reset();
		seqY.Reset();
	} else if(resetAccess.process(inputs[INIT_IN].value))
	{
		pWidget->resetAccess();
	} else
	{
		if(pWidget != NULL)
		{
			if(accessRndTrigger.process(inputs[RANDOMIZONE].value))
				randrandrand();
		}

		float rec_smp;
		int rec_step;
		if(cvs.IsRecAvailable(&rec_smp, &rec_step))
		{
			pWidget->SetValue(Renato::VOLTAGE_1 + rec_step, rec_smp);
		}

		bool seek_mode = isSwitchOn(this, SEEKSLEEP);
		int clkX = seqX.Step(inputs[XCLK].getVoltage(), params[COUNTMODE_X].getValue(), seek_mode, this, true);
		int clkY = seqY.Step(inputs[YCLK].getVoltage(), params[COUNTMODE_Y].getValue(), seek_mode, this, false);
		int n = xy(seqX.Position(), seqY.Position());

		if(_access(n))
		{
			bool on = false;
			if(_gateX(n))
			{
				if(seqX.Gate(clkX, &outputs[XGATE], &lights[LED_GATEX]))
					on = true;
			}

			if(_gateY(n))
			{
				if(seqY.Gate(clkY, &outputs[YGATE], &lights[LED_GATEY]))
					on = true;
			}

			outputs[CV].value = cvs.TransposeableValue(params[VOLTAGE_1 + n].value);
			setOut(n, on);
			led(n);
		}
	}
	#ifdef DIGITAL_EXT
	bool dig_connected = false;

	#ifdef LAUNCHPAD
	if(drv->Connected())
		dig_connected = true;
	drv->ProcessLaunchpad();
	#endif

	#if defined(OSCTEST_MODULE)
	if(oscDrv->Connected())
		dig_connected = true;
	oscDrv->ProcessOSC();
	#endif	
	connected = dig_connected ? 1.0 : 0.0;
	#endif
}

Menu *RenatoWidget::addContextMenu(Menu *menu)
{
	menu->addChild(new RandomizeItem(module));

	menu->addChild(new SeqMenuItem<RenatoWidget>("Randomize Pitch", this, RANDOMIZE_PITCH));
	menu->addChild(new SeqMenuItem<RenatoWidget>("Randomize Gate Xs", this, RANDOMIZE_GATEX));
	menu->addChild(new SeqMenuItem<RenatoWidget>("Randomize Gate Ys", this, RANDOMIZE_GATEY));
	menu->addChild(new SeqMenuItem<RenatoWidget>("Randomize Access", this, RANDOMIZE_ACCESS));
	menu->addChild(new SeqMenuItem<RenatoWidget>("Pitch Quantization", this, QUANTIZE_PITCH));
	return menu;
}

void Renato::randrandrand()
{
	if(theRandomizer & RenatoWidget::RANDOMIZE_PITCH)
		randrandrand(0);

	if(theRandomizer & RenatoWidget::RANDOMIZE_GATEX)
		randrandrand(1);

	if(theRandomizer & RenatoWidget::RANDOMIZE_GATEY)
		randrandrand(2);

	if(theRandomizer & RenatoWidget::RANDOMIZE_ACCESS)
		randrandrand(3);

	if(theRandomizer & RenatoWidget::RANDOMIZE_LAQUALUNQUE)
	{
		randrandrand(int(random::uniform() * 4));
	}
}

void Renato::randrandrand(int action)
{
	switch(action)
	{
		case 0: pWidget->std_randomize(Renato::VOLTAGE_1, Renato::VOLTAGE_1 + 16); break;
		case 1: pWidget->std_randomize(Renato::GATEX_1, Renato::GATEX_1 + 16); break;
		case 2: pWidget->std_randomize(Renato::GATEY_1, Renato::GATEY_1 + 16); break;
		case 3: pWidget->std_randomize(Renato::ACCESS_1, Renato::ACCESS_1 + 16); break;
	}

}

void Renato::QuantizePitch()
{
	for(int k = 0; k < 16; k++)
		params[VOLTAGE_1 + k].value = pWidget->quantizePitch(VOLTAGE_1 + k, params[VOLTAGE_1 + k].value, cvs);
}

void RenatoWidget::onMenu(int action)
{
	switch(action)
	{
		case RANDOMIZE_PITCH:  std_randomize(Renato::VOLTAGE_1, Renato::VOLTAGE_1 + 16); break;
		case RANDOMIZE_GATEX:  std_randomize(Renato::GATEX_1, Renato::GATEX_1 + 16); break;
		case RANDOMIZE_GATEY:  std_randomize(Renato::GATEY_1, Renato::GATEY_1 + 16); break;
		case RANDOMIZE_ACCESS: std_randomize(Renato::ACCESS_1, Renato::ACCESS_1 + 16); break;
		case QUANTIZE_PITCH: ((Renato *)module)->QuantizePitch(); break;

	}
}

RenatoWidget::RenatoWidget(Renato *module) : SequencerWidget()
{
	if(module != NULL)
		module->setWidget(this);

	#ifdef OSCTEST_MODULE
	char name[60];
	#endif

	CREATE_PANEL(module, this, 41, "res/modules/RenatoModule.svg");

	addInput(createInput<PJ301RPort>(Vec(mm2px(33.509), yncscape(115.267, 8.255)), module, Renato::XCLK));
	addInput(createInput<PJ301RPort>(Vec(mm2px(49.222), yncscape(115.267, 8.255)), module, Renato::YCLK));
	addInput(createInput<PJ301YPort>(Vec(mm2px(119.500), yncscape(115.267, 8.255)), module, Renato::RESET));
	addInput(createInput<PJ301BPort>(Vec(mm2px(134.122), yncscape(115.267, 8.255)), module, Renato::INIT_IN));

	addInput(createInput<PJ301HPort>(Vec(mm2px(12.472), yncscape(115.267, 8.255)), module, Renato::RANDOMIZONE));

	addChild(createParam<BefacoPushBig>(Vec(mm2px(108.494), yncscape(114.895, 8.999)), module, Renato::M_RESET));

	if(module != NULL)
		module->cvs.Create(this, 196.797f, 18.530f, Renato::NUM_INPUTS - cvStrip::CVSTRIP_INPUTS, Renato::NUM_PARAMS - cvStrip::CVSTRIP_PARAMS, 16);

	// page 0 (SESSION)
	ParamWidget *pwdg = createParam<NKK2>(Vec(mm2px(60.319), yncscape(115.727 + 1, 8.467)), module, Renato::COUNTMODE_X);
	addParam(pwdg);
	#ifdef LAUNCHPAD
	if(module != NULL)
	{
		LaunchpadRadio *radio = new LaunchpadRadio(0, ILaunchpadPro::RC2Key(2, 1), 3, LaunchpadLed::Color(47), LaunchpadLed::Color(32));
		module->drv->Add(radio, pwdg);
	}
	#endif
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		module->oscDrv->Add(new oscControl("/ModeX"), pwdg);
	}
	#endif

	pwdg = createParam<NKK2>(Vec(mm2px(84.044), yncscape(115.727 + 1, 8.467)), module, Renato::COUNTMODE_Y);
	addParam(pwdg);
	#ifdef LAUNCHPAD
	if(module != NULL)
	{
		module->drv->Add(new LaunchpadRadio(0, ILaunchpadPro::RC2Key(2, 3), 3, LaunchpadLed::Color(19), LaunchpadLed::Color(21)), pwdg);
	}
	#endif
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		module->oscDrv->Add(new oscControl("/ModeY"), pwdg);
	}
	#endif

	pwdg = createParam<NKK1>(Vec(mm2px(96.126), yncscape(115.727 + 1, 8.467)), module, Renato::SEEKSLEEP);
	addParam(pwdg);
	#ifdef LAUNCHPAD
	if(module != NULL)
	{
		module->drv->Add(new LaunchpadRadio(0, ILaunchpadPro::RC2Key(2, 5), 2, LaunchpadLed::Color(51), LaunchpadLed::Color(52)), pwdg);
	}
	#endif
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		module->oscDrv->Add(new oscControl("/Seek"), pwdg);
	}
	#endif

	addOutput(createOutput<PJ301GPort>(Vec(mm2px(181.436), yncscape(115.267, 8.255)), module, Renato::CV));
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(152.891), yncscape(115.267, 8.255)), module, Renato::XGATE));
	ModuleLightWidget *plight = createLight<MediumLight<GreenLight>>(Vec(mm2px(160.534), yncscape(112.637, 3.176)), module, Renato::LED_GATEX);
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		module->oscDrv->Add(new oscControl("/LedGX"), plight);
	}
	#endif
	addChild(plight);

	addOutput(createOutput<PJ301WPort>(Vec(mm2px(171.033), yncscape(115.267, 8.255)), module, Renato::YGATE));
	plight = createLight<MediumLight<GreenLight>>(Vec(mm2px(168.403), yncscape(112.637, 3.176)), module, Renato::LED_GATEY);
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		module->oscDrv->Add(new oscControl("/LedGY"), plight);
	}
	#endif
	addChild(plight);

	// page 1 (NOTES)
	float groupdist_h = 47.343;
	float groupdist_v = -27.8;
	float x_sup[4] = {7.899, 18.293, 28.687, 39.330};
	float x_inf[4] = {9.788, 20.182, 30.576, 38.695};
	float x_led = 46.981;
	float y_led = 97.848;
	float y_sup = 100.419;
	float y_inf = 92.021;
	float y_pot = 89.561;

	for(int r = 0; r < 4; r++)
	{
		for(int c = 0; c < 4; c++)
		{
			int n = c + r * 4;
			addInput(createInput<PJ301BPort>(Vec(mm2px(x_sup[0] + c * groupdist_h), yncscape(y_sup + r * groupdist_v, 8.255)), module, Renato::ACCESS_IN1 + n));
			addInput(createInput<PJ301BPort>(Vec(mm2px(x_sup[1] + c * groupdist_h), yncscape(y_sup + r * groupdist_v, 8.255)), module, Renato::GATEX_IN1 + n));
			addInput(createInput<PJ301BPort>(Vec(mm2px(x_sup[2] + c * groupdist_h), yncscape(y_sup + r * groupdist_v, 8.255)), module, Renato::GATEY_IN1 + n));
			addOutput(createOutput<PJ301WPort>(Vec(mm2px(x_sup[3] + c * groupdist_h), yncscape(y_sup + r * groupdist_v, 8.255)), module, Renato::CV_OUTSTEP1 + n));

			ParamWidget *pwdg = createParam<TL1105Sw>(Vec(mm2px(x_inf[0] + c * groupdist_h), yncscape(y_inf + r * groupdist_v, 6.607)), module, Renato::ACCESS_1 + n);
			addParam(pwdg);
			#ifdef LAUNCHPAD
			if(module != NULL)
			{
				module->drv->Add(new LaunchpadSwitch(1, ILaunchpadPro::RC2Key(r + 4, c), LaunchpadLed::Off(), LaunchpadLed::Color(17)), pwdg);
			}
			#endif
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/Access%i", n + 1);
				module->oscDrv->Add(new oscControl(name), pwdg);
			}
			#endif

			pwdg = createParam<TL1105Sw>(Vec(mm2px(x_inf[1] + c * groupdist_h), yncscape(y_inf + r * groupdist_v, 6.607)), module, Renato::GATEX_1 + n);
			addParam(pwdg);
			#ifdef LAUNCHPAD
			if(module != NULL)
			{
				module->drv->Add(new LaunchpadSwitch(1, ILaunchpadPro::RC2Key(r + 4, c + 4), LaunchpadLed::Off(), LaunchpadLed::Color(52)), pwdg);
			}
			#endif
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/GateX%i", n + 1);
				module->oscDrv->Add(new oscControl(name), pwdg);
			}
			#endif

			pwdg = createParam<TL1105Sw>(Vec(mm2px(x_inf[2] + c * groupdist_h), yncscape(y_inf + r * groupdist_v, 6.607)), module, Renato::GATEY_1 + n);
			addParam(pwdg);
			#ifdef LAUNCHPAD
			if(module != NULL)
			{
				module->drv->Add(new LaunchpadSwitch(1, ILaunchpadPro::RC2Key(r, c + 4), LaunchpadLed::Off(), LaunchpadLed::Color(62)), pwdg);
			}
			#endif
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/GateY%i", n + 1);
				module->oscDrv->Add(new oscControl(name), pwdg);
			}
			#endif

			pwdg = createParam<Davies1900hFixRedKnob>(Vec(mm2px(x_inf[3] + c * groupdist_h), yncscape(y_pot + r * groupdist_v, 9.525)), module, Renato::VOLTAGE_1 + n);
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/Knob%i", n + 1);
				module->oscDrv->Add(new oscControl(name), pwdg);
			}
			#endif
			addParam(pwdg);

			ModuleLightWidget *plight = createLight<MediumLight<RedLight>>(Vec(mm2px(x_led + c * groupdist_h), yncscape(y_led + r * groupdist_v, 3.176)), module, Renato::LED_1 + n);
			addChild(plight);
			#ifdef LAUNCHPAD
			if(module != NULL)
			{
				LaunchpadLight *ld1 = new LaunchpadLight(1, ILaunchpadPro::RC2Key(r, c), LaunchpadLed::Off(), LaunchpadLed::Color(4));
				module->drv->Add(ld1, plight);
			}
			#endif
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/Led%i", n + 1);
				module->oscDrv->Add(new oscControl(name), plight);
			}
			#endif
		}
	}

	#ifdef DIGITAL_EXT
	if(module != NULL)
		addChild(new DigitalLed(mm2px(107.531), yncscape(117.461, 3.867), &module->connected));
	#endif
}

RenatoWidget::RandomizeSubItemItem::RandomizeSubItemItem(Module *k, const char *title, int action)
{
	renato = (Renato *)k;
	text = title;
	randomizeDest = action;
	rightText = CHECKMARK((renato->theRandomizer & randomizeDest) != 0);
}

void RenatoWidget::RandomizeSubItemItem::onAction(const event::Action &e)
{
	renato->theRandomizer ^= randomizeDest;
}

void RenatoWidget::resetAccess()
{
	for(int k = 0; k < 16; k++)
	{
		SetValue(Renato::ACCESS_1 + k, 1);
		SetValue(Renato::GATEX_1 + k, 1);
		SetValue(Renato::GATEY_1 + k, 1);
	}
}
