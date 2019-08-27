#pragma once
struct portSmall : _ioPort
{
	portSmall() : _ioPort("res/PJ301Bsmall.svg") {}
};

struct daviesVerySmall : _davies1900base
{
	daviesVerySmall() : _davies1900base("res/Davies1900hBlackVerySmall.svg") {}
	void randomize() override
	{
		// do NOT randomaiz
	}
};

struct nag7Segm : TransparentWidget
{
private:
	std::shared_ptr<Font> font;
	NagSeq *pSeq;
	nag *pNag;
	void init(float x, float y)
	{
		box.size = Vec(27, 22);
		box.pos = Vec(mm2px(x), yncscape(y, px2mm(box.size.y)));
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
	}

public:
	nag7Segm(NagSeq *sq, float x, float y)
	{
		pSeq = sq;
		pNag = NULL;
		init(x, y);
	}
	nag7Segm(nag *p, float x, float y)
	{
		pSeq = NULL;
		pNag = p;
		init(x, y);
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
			if(pSeq->enabled)
			{
				char n[20];
				sprintf(n, "%2i", pSeq->numVertici);
				textColor = nvgRGB(0xff, 0x00, 0x00);
				nvgFillColor(args.vg, textColor);
				nvgText(args.vg, textPos.x, textPos.y, n, NULL);
			} else
			{
				textColor = nvgRGB(0x9f, 0x00, 0x00);
				nvgFillColor(args.vg, textColor);
				nvgText(args.vg, textPos.x, textPos.y, "--", NULL);
			}
		} else if(pNag != NULL)
		{
			char n[20];
			sprintf(n, "%2i", pNag->degPerClock());
			textColor = nvgRGB(0xff, 0x00, 0x00);
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, textPos.x, textPos.y, n, NULL);
		}
	}
};
struct outputRange : Widget
{
public:
	outputRange()
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/outputRange.svg")));
	}

	static const int NUMINPUTS = 4;
	static const int RANGE_FROM = 0;
	static const int RANGE_TO = 1;
	static const int REC_TRANSPOSE = 2;
	static const int REC_GATE - 3;
	static const int NUMPARAMS = 5;
	static const int REC_MODE = 2;

	void Create(ModuleWidget *pWidget, float x, float y, int port, int param)
	{
		portID = port;
		paramID = param;
		pWidget->addInput(createInput<portSmall>(Vec(mm2px(x + 1.072f),  yncscape(y , 59.367f+0.595f)), pWidget->module, portID+ RANGE_FROM));
		pWidget->addInput(createInput<portSmall>(Vec(mm2px(x + 1.072f), yncscape(y , 43.368f+0.595f)), pWidget->module, portID + RANGE_TO));
		pWidget->addInput(createInput<portSmall>(Vec(mm2px(x + 1.072f), yncscape(y, 33.377f + 0.595f)), pWidget->module, portID + REC_TRANSPOSE));
		pWidget->addInput(createInput<portSmall>(Vec(mm2px(x + 1.072f), yncscape(y ,1.704f+0.595f)), pWidget->module, portID + REC_GATE));

		ParamWidget *pwdg = createParam<daviesVerySmall>(Vec(mm2px(x + 1.015f), yncscape(y+66.310f , 1.704.f)), pWidget->module, paramID+ RANGE_FROM);
		pWidget->addParam(pwdg);
		#ifdef OSCTEST_MODULE
		if(pWidget->module != NULL)
			pWidget->module->oscDrv->Add(new oscControl("RangeMin"), pwdg);
		#endif	
		pwdg = createParam<daviesVerySmall>(Vec(mm2px(x + 0.712f), yncscape(y, 25.617f + 6.f)), pWidget->module, paramID + RANGE_TO);
		pWidget->addParam(pwdg);
		#ifdef OSCTEST_MODULE
		if(pWidget->module != NULL)
			pWidget->module->oscDrv->Add(new oscControl("RangeMax"), pwdg);
		#endif

		pwdg = createParam<TL1105HSwRed>(Vec(mm2px(x + 1.015f), yncscape(y, 25.617f + 4.477f)), pWidget->module, paramID + 2);
		pwdg->randomizable = false;
		pWidget->addParam(pwdg);
		#ifdef OSCTEST_MODULE
		if(pWidget->module != NULL)
			pWidget->module->oscDrv->Add(new oscControl("Rec"), pwdg);
		#endif
	}

	float TransposeableValue(float v)
	{
		if(module != NULL && portID >= 0)
		{
			float trnsps;
			if(recording)
				trnsps = 0.f;
			else
				trnsps = module->inputs[portID+2].getNormalVoltage(MIDDLE_C) - MIDDLE_C;

			float vmin = clamp(module->params[paramID].value + module->inputs[portID+ RANGE_FROM].getNormalVoltage(0.0), LVL_MIN, LVL_MAX);
			float vmax = clamp(module->params[paramID + 1].value + module->inputs[portID + RANGE_TO].getNormalVoltage(0.0), LVL_MIN, LVL_MAX);
			float vnt = rescale(v, 0.0, 1.0, std::min(vmin, vmax), std::max(vmin, vmax));
			return clamp(vnt + trnsps, LVL_MIN, LVL_MAX);
		}
		return 0;
	}

	bool IsRecAvailable(float *smpl)
	{
		if(module != NULL && portID >= 0)
		{
			int stat = recTrig.process(module->params[paramID+2].value);
			if(clk == 1)	// transizione off->on
			{
				recording = true;
			} else if(clk == -1)// transizione on->off
			{
				recording = false;
			}
		
			if(recording)
				*smpl = Reverse(module->inputs[portID+2].getNormalVoltage(0.0));
		}

		return recording;
	}
	
	
	float Value(float v) const //v normalizzato 0-1
	{
		if(module != NULL && portID >= 0)
		{
			float vmin = clamp(module->params[paramID].value + module->inputs[portID+RANGE_FROM].getNormalVoltage(0.0), LVL_MIN, LVL_MAX);
			float vmax = clamp(module->params[paramID + 1].value + module->inputs[portID + RANGE_TO].getNormalVoltage(0.0), LVL_MIN, LVL_MAX);
			return clamp(rescale(v, 0.0, 1.0, std::min(vmin, vmax), std::max(vmin, vmax)), LVL_MIN, LVL_MAX);
		}
		return 0;
	}

	float Reverse(float v) const
	{
		if(module != NULL && portID >= 0)
		{
			float vmin = clamp(module->params[paramID].value + module->inputs[portID + RANGE_FROM].getNormalVoltage(0.0), LVL_MIN, LVL_MAX);
			float vmax = clamp(module->params[paramID + 1].value + module->inputs[portID + RANGE_TO].getNormalVoltage(0.0), LVL_MIN, LVL_MAX);
			return clamp(rescale(v, std::min(vmin, vmax), std::max(vmin, vmax), 0.0, 1.0), 0.0, 1.0);
		}

		return v;
	}

	void configure(Module *pModule, int param)
	{
		module = pModule;
		paramID = param;
		module->configParam(paramID, LVL_MIN, LVL_MAX, 0.0, "Output Range: Min Voltage", "V");
		module->configParam(paramID + 1, LVL_MIN, LVL_MAX, LVL_MAX / 2.f, "Output Range: Max Voltage", "V");
	}

private:
	int portID = -1;
	int paramID = -1;
	Module *module = NULL;
	SchmittTrigger2 recTrig;
	bool recording = false;
};