#include "../include/SpiraloneModule.hpp"
#include <math.h>


struct spiro7Segm : TransparentWidget
{
private:
	std::shared_ptr<Font> font;
	Spiralone *pSeq;
	int seq;

public:
	spiro7Segm(Spiralone *sq, int n, float x, float y)
	{
		seq = n;
		pSeq = sq;
		box.size = Vec(27, 22);
		box.pos = Vec(mm2px(x), yncscape(y, px2mm(box.size.y)));
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
	}

	void draw(const DrawArgs &args) override
	{
		// Background
		NVGcolor backgroundColor = nvgRGB(0x20, 0x20, 0x20);
		NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
		nvgFillColor(args.vg, backgroundColor);
		nvgFill(args.vg);
		nvgStrokeWidth(args.vg, 1.0);
		nvgStrokeColor(args.vg, borderColor);
		nvgStroke(args.vg);
		// text
		nvgFontSize(args.vg, 18);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 2.5);

		Vec textPos = Vec(2, 18);
		NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
		nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
		nvgText(args.vg, textPos.x, textPos.y, "~~", NULL);

		textColor = nvgRGB(0xda, 0xe9, 0x29);
		nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
		nvgText(args.vg, textPos.x, textPos.y, "\\\\", NULL);

		if(pSeq != NULL)
		{
			char n[20];
			sprintf(n, "%2i", pSeq->sequencer[seq].GetNumSteps(pSeq));
			textColor = nvgRGB(0xff, 0x00, 0x00);
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, textPos.x, textPos.y, n, NULL);
		}
	}
};

float AccessParam(Spiralone *p, int seq, int id) { return p->params[id + seq].value; }
float AccessParam(Spiralone *p, int id) { return p->params[id].value; }
Input *AccessInput(Spiralone *p, int seq, int id) { return &p->inputs[id + seq]; }
float *AccessOutput(Spiralone *p, int seq, int id) { return &p->outputs[id + seq].value; }
float *AccessLight(Spiralone *p, int id) { return &p->lights[id].value; }

void Spiralone::on_loaded()
{
	#ifdef DIGITAL_EXT
	connected = 0;
	#endif
	load();
}

void Spiralone::load()
{
	for(int k = 0; k < NUM_SEQUENCERS; k++)
		sequencer[k].Reset(this);
}

void Spiralone::process(const ProcessArgs &args)
{
	if(masterReset.process(params[M_RESET].value))
	{
		load();
	} else
	{
		if(pWidget != NULL)
		{
			if(rndTrigger.process(inputs[RANDOMIZONE].value))
				randrandrand();
		}

		for(int k = 0; k < NUM_SEQUENCERS; k++)
			sequencer[k].Step(this);
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

void Spiralone::randrandrand()
{
	if(theRandomizer & SpiraloneWidget::RANDOMIZE_PITCH)
		randrandrand(0);

	if(theRandomizer & SpiraloneWidget::RANDOMIZE_LEN)
		randrandrand(1);

	if(theRandomizer & SpiraloneWidget::RANDOMIZE_STRIDE)
		randrandrand(2);

	if(theRandomizer & SpiraloneWidget::RANDOMIZE_XPOSE)
		randrandrand(3);

	if(theRandomizer & SpiraloneWidget::RANDOMIZE_MODE)
		randrandrand(4);

	if(theRandomizer & SpiraloneWidget::RANDOMIZE_LAQUALUNQUE)
	{
		randrandrand(int(random::uniform() * 5));
	}
}

void Spiralone::randrandrand(int action)
{
	switch(action)
	{
		case 0:
			pWidget->std_randomize(Spiralone::VOLTAGE_1, Spiralone::VOLTAGE_1 + TOTAL_STEPS);
			break;

		case 1:
			pWidget->std_randomize(Spiralone::LENGHT_1, Spiralone::LENGHT_1 + NUM_SEQUENCERS);
			break;

		case 2:
			pWidget->std_randomize(Spiralone::STRIDE_1, Spiralone::STRIDE_1 + NUM_SEQUENCERS);
			break;

		case 3:
			pWidget->std_randomize(Spiralone::XPOSE_1, Spiralone::XPOSE_1 + NUM_SEQUENCERS);
			break;

		case 4:
			pWidget->std_randomize(Spiralone::MODE_1, Spiralone::MODE_1 + NUM_SEQUENCERS);
			break;
	}
}

void Spiralone::QuantizePitch()
{
	for(int k = 0; k < TOTAL_STEPS; k++)
		params[VOLTAGE_1 + k].value = pWidget->quantizePitch(VOLTAGE_1 + k, params[VOLTAGE_1 + k].value, cvs);
}

SpiraloneWidget::SpiraloneWidget(Spiralone *module) : SequencerWidget()
{
	if(module != NULL)
		module->setWidget(this);

	#ifdef OSCTEST_MODULE
	char name[60];
	#endif

	color[0] = componentlibrary::SCHEME_RED;
	color[1] = componentlibrary::SCHEME_WHITE;
	color[2] = componentlibrary::SCHEME_BLUE;
	color[3] = componentlibrary::SCHEME_YELLOW;
	color[4] = componentlibrary::SCHEME_GREEN;

	CREATE_PANEL(module, this, 53, "res/modules/SpiraloneModule.svg");

	float step = 2 * M_PI / TOTAL_STEPS;
	float angle = M_PI / 2.0;
	for(int k = 0; k < TOTAL_STEPS; k++)
	{
		float r = 56.0;
		float cx = cos(angle);
		float cy = sin(angle);
		float center_y = 64.250;
		float center_x = 66.894;

		ParamWidget *pctrl = createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(center_x - 4.0 + r * cx), yncscape(center_y - 4.0 + r * cy, 8.0)), module, Spiralone::VOLTAGE_1 + k);
		#ifdef OSCTEST_MODULE
		if(module != NULL)
		{
			sprintf(name, "/Knob%i", k + 1);
			module->oscDrv->Add(new oscControl(name), pctrl);
		}
		#endif
		addParam(pctrl);

		r -= 2;
		for(int s = 0; s < NUM_SEQUENCERS; s++)
		{
			int n = s * TOTAL_STEPS + k;
			r -= 6;
			ModuleLightWidget *plight = createLed(s, Vec(mm2px(center_x - 1.088 + r * cx), yncscape(center_y - 1.088 + r * cy, 2.176)), module, Spiralone::LED_SEQUENCE_1 + n);
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/Led%i_%i", s + 1, n + 1);
				module->oscDrv->Add(new oscControl(name), plight);
			}
			#endif
			addChild(plight);

			if(k == 0)
				createSequencer(s);
		}
		angle += step;
	}

	ParamWidget *pwdg = createParam<BefacoPushBig>(Vec(mm2px(7.970), yncscape(113.627, 8.999)), module, Spiralone::M_RESET);
	addParam(pwdg);
	#ifdef LAUNCHPAD
	if(module != NULL)
	{
		module->drv->Add(new LaunchpadMomentary(0, ILaunchpadPro::RC2Key(0, 7), LaunchpadLed::Color(63), LaunchpadLed::Color(62)), pwdg);
	}
	#endif
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		module->oscDrv->Add(new oscControl("/Reset"), pwdg);
	}
	#endif
	addInput(createInput<PJ301HPort>(Vec(mm2px(62.766), yncscape(59.593, 8.255)), module, Spiralone::RANDOMIZONE));

	if(module != NULL)
		module->cvs.Create(this, 258.022f, 41.284f, ascii::NUM_INPUTS - cvMiniStrip::CVMINISTRIP_INPUTS, ascii::NUM_PARAMS - cvMiniStrip::CVMINISTRIP_PARAMS);

	#ifdef DIGITAL_EXT
	if(module != NULL)
		addChild(new DigitalLed(mm2px(112.474), yncscape(4.428, 3.867), &module->connected));
	#endif
}

void SpiraloneWidget::createSequencer(int seq)
{
	#ifdef OSCTEST_MODULE
	char name[60];
	#endif

	float dist_v = -25.206;

	addInput(createInput<PJ301RPort>(Vec(mm2px(143.251), yncscape(115.825 + dist_v * seq, 8.255)), module, Spiralone::CLOCK_1 + seq));
	addInput(createInput<PJ301YPort>(Vec(mm2px(143.251), yncscape(104.395 + dist_v * seq, 8.255)), module, Spiralone::RESET_1 + seq));

	ParamWidget *pwdg = createParam<NKK2>(Vec(mm2px(158.607), yncscape(109.773 + dist_v * seq, 9.488)), module, Spiralone::MODE_1 + seq);
	addParam(pwdg);
	#ifdef LAUNCHPAD
	if(module != NULL)
	{
		int color_launchpad[NUM_SEQUENCERS][2];
		color_launchpad[0][0] = 11; color_launchpad[0][1] = 5;
		color_launchpad[1][0] = 1; color_launchpad[1][1] = 3;
		color_launchpad[2][0] = 47; color_launchpad[2][1] = 37;
		color_launchpad[3][0] = 15; color_launchpad[3][1] = 12;
		color_launchpad[4][0] = 19; color_launchpad[4][1] = 21;
		LaunchpadRadio *sw = new LaunchpadRadio(0, 0, ILaunchpadPro::RC2Key(0, seq), 3, LaunchpadLed::Color(color_launchpad[seq][0]), LaunchpadLed::Color(color_launchpad[seq][1]));
		((Spiralone *)module)->drv->Add(sw, pwdg);
	}
	#endif
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		sprintf(name, "/Mode%i", seq + 1);
		((Spiralone *)module)->oscDrv->Add(new oscControl(name), pwdg);
	}
	#endif

	pwdg = createParam<Davies1900hFixRedKnobSmall>(Vec(mm2px(175.427), yncscape(115.953 + dist_v * seq, 8.0)), module, Spiralone::LENGHT_1 + seq);
	((Davies1900hKnob *)pwdg)->snap = true;
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		sprintf(name, "/Lenght%i", seq + 1);
		((Spiralone *)module)->oscDrv->Add(new oscControl(name), pwdg);
	}
	#endif
	addParam(pwdg);
	addInput(createInput<PJ301BPort>(Vec(mm2px(182.178), yncscape(104.395 + dist_v * seq, 8.0)), module, Spiralone::INLENGHT_1 + seq));

	pwdg = createParam<Davies1900hFixBlackKnobSmall>(Vec(mm2px(195.690), yncscape(115.953 + dist_v * seq, 8.255)), module, Spiralone::STRIDE_1 + seq);
	((Davies1900hKnob *)pwdg)->snap = true;
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		sprintf(name, "/Stride%i", seq + 1);
		((Spiralone *)module)->oscDrv->Add(new oscControl(name), pwdg);
	}
	#endif
	addParam(pwdg);
	addInput(createInput<PJ301BPort>(Vec(mm2px(201.913), yncscape(104.395 + dist_v * seq, 8.255)), module, Spiralone::INSTRIDE_1 + seq));

	pwdg = createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(215.954), yncscape(115.953 + dist_v * seq, 8.0)), module, Spiralone::XPOSE_1 + seq);
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		sprintf(name, "/Transpose%i", seq + 1);
		((Spiralone *)module)->oscDrv->Add(new oscControl(name), pwdg);
	}
	#endif
	addParam(pwdg);
	addInput(createInput<PJ301BPort>(Vec(mm2px(222.177), yncscape(104.395 + dist_v * seq, 8.255)), module, Spiralone::INXPOSE_1 + seq));

	addOutput(createOutput<PJ301GPort>(Vec(mm2px(238.996), yncscape(115.825 + dist_v * seq, 8.255)), module, Spiralone::CV_1 + seq));
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(238.996), yncscape(104.395 + dist_v * seq, 8.255)), module, Spiralone::GATE_1 + seq));

	addChild(new spiro7Segm((Spiralone *)module, seq, 169.177, 104.698 + dist_v * seq));
}

ModuleLightWidget *SpiraloneWidget::createLed(int seq, Vec pos, Module *module, int firstLightId, bool big)
{
	ModuleLightWidget *rv = new ModuleLightWidget();
	if(big)
		rv->box.size = mm2px(Vec(3, 3));
	else
		rv->box.size = mm2px(Vec(2.176, 2.176));
	rv->box.pos = pos;
	rv->addBaseColor(color[seq]);
	rv->module = module;
	rv->firstLightId = firstLightId;
	//rv->bgColor = COLOR_BLACK_TRANSPARENT;
	return rv;
}

Menu *SpiraloneWidget::addContextMenu(Menu *menu)
{
	menu->addChild(new RandomizeItem(module));

	menu->addChild(new SeqMenuItem<SpiraloneWidget>("Randomize Pitch", this, RANDOMIZE_PITCH));
	menu->addChild(new SeqMenuItem<SpiraloneWidget>("Randomize Length", this, RANDOMIZE_LEN));
	menu->addChild(new SeqMenuItem<SpiraloneWidget>("Randomize Stride", this, RANDOMIZE_STRIDE));
	menu->addChild(new SeqMenuItem<SpiraloneWidget>("Randomize Transpose", this, RANDOMIZE_XPOSE));
	menu->addChild(new SeqMenuItem<SpiraloneWidget>("Randomize Mode", this, RANDOMIZE_MODE));
	menu->addChild(new SeqMenuItem<SpiraloneWidget>("Pitch Quantization", this, QUANTIZE_PITCH));

	return menu;
}

void SpiraloneWidget::onMenu(int action)
{
	switch(action)
	{
		case RANDOMIZE_PITCH:
			std_randomize(Spiralone::VOLTAGE_1, Spiralone::VOLTAGE_1 + TOTAL_STEPS);
			break;

		case RANDOMIZE_LEN:
			std_randomize(Spiralone::LENGHT_1, Spiralone::LENGHT_1 + NUM_SEQUENCERS);
			break;

		case RANDOMIZE_STRIDE:
			std_randomize(Spiralone::STRIDE_1, Spiralone::STRIDE_1 + NUM_SEQUENCERS);
			break;

		case RANDOMIZE_XPOSE:
			std_randomize(Spiralone::XPOSE_1, Spiralone::XPOSE_1 + NUM_SEQUENCERS);
			break;

		case RANDOMIZE_MODE:
			std_randomize(Spiralone::MODE_1, Spiralone::MODE_1 + NUM_SEQUENCERS);
			break;

		case QUANTIZE_PITCH: ((Spiralone *)module)->QuantizePitch(); break;

	}
}

SpiraloneWidget::RandomizeSubItemItem::RandomizeSubItemItem(Module *spir, const char *title, int action)
{
	spiro = (Spiralone *)spir;
	text = title;
	randomizeDest = action;
	rightText = CHECKMARK((spiro->theRandomizer & randomizeDest) != 0);
}

void SpiraloneWidget::RandomizeSubItemItem::onAction(const event::Action &e)
{
	spiro->theRandomizer ^= randomizeDest;
}
