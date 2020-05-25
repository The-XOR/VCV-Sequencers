#pragma once

#define RANGE_FROM		(portID)
#define RANGE_TO		(portID+1)
#define CV_IN			(portID+2)
#define GATE_IN			(portID+3)
#define PARAM_FROM		(paramID)
#define PARAM_TO  		(paramID+1)
#define PARAM_REC 		(paramID+2)
#define PARAM_MANU		(paramID+3)
#define PARAM_NEXT		(paramID+4)
#define PARAM_PREV      (paramID+5)

// 10.160 x 33.728 mm

struct cvMicroStrip
{
public:
	static const int CVMICROSTRIP_INPUTS = 2;
	static const int CVMICROSTRIP_PARAMS = 2;

	cvMicroStrip()
	{

	}

	void Create(ModuleWidget *pWidget, float x, float y, int port, int param)
	{
		portID = port;
		paramID = param;

		SvgWidget *pw = createWidget<SvgWidget>(Vec(mm2px(x), yncscape(y, 33.728f)));
		pw->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/cvMicroStrip.svg")));
		pw->wrap();
		pWidget->addChild(pw);

		addMicroStrip(pWidget, x, y, -57.712f);
	}

	void configure(Module *pModule, int param)
	{
		module = pModule;
		paramID = param;
		module->configParam(PARAM_FROM, LVL_MIN, LVL_MAX, -3.f, "Output Range: Min Voltage", "V");
		module->configParam(PARAM_TO, LVL_MIN, LVL_MAX, 3.f, "Output Range: Max Voltage", "V");
	}

	float Value(float v) const //v normalizzato 0-1
	{
		float vmin = getModulableParam(module, PARAM_FROM, RANGE_FROM, LVL_MIN, LVL_MAX);
		float vmax = getModulableParam(module, PARAM_TO, RANGE_TO, LVL_MIN, LVL_MAX);
		return clamp(rescale(v, 0.0, 1.0, std::min(vmin, vmax), std::max(vmin, vmax)), LVL_MIN, LVL_MAX);
	}

	float Reverse(float v) const
	{
		float vmin = getModulableParam(module, PARAM_FROM, RANGE_FROM, LVL_MIN, LVL_MAX);
		float vmax = getModulableParam(module, PARAM_TO, RANGE_TO, LVL_MIN, LVL_MAX);
		return clamp(rescale(v, std::min(vmin, vmax), std::max(vmin, vmax), 0.0, 1.0), 0.0, 1.0);
	}

protected:
	void addMicroStrip(ModuleWidget *pWidget, float x, float y, float corr)
	{
		pWidget->addInput(createInput<portSmall>(Vec(pos_x(x, 1.072f), pos_y(y, corr + 74.942f, 5.885f)), pWidget->module, RANGE_FROM));
		pWidget->addInput(createInput<portSmall>(Vec(pos_x(x, 1.072f), pos_y(y, corr + 58.942f, 5.885f)), pWidget->module, RANGE_TO));

		ParamWidget *pwdg = createParam<daviesVerySmall>(Vec(pos_x(x, 1.015f), pos_y(y, corr + 81.885f, 6.f)), pWidget->module, PARAM_FROM);
		pWidget->addParam(pwdg);

		pwdg = createParam<daviesVerySmall>(Vec(pos_x(x, 1.015f), pos_y(y, corr + 65.886f, 6.f)), pWidget->module, PARAM_TO);
		pWidget->addParam(pwdg);
	}

	float pos_x(float x, float offs)
	{
		return mm2px(x + offs);
	}

	float pos_y(float y, float offs, float height)
	{
		return yncscape(y + offs, height);
	}

protected:
	int portID = -1;
	int paramID = -1;
	Module *module = NULL;
};

// 10.160 x 45.932 mm
struct cvMiniStrip : cvMicroStrip
{
	public:
	static const int CVMINISTRIP_INPUTS = CVMICROSTRIP_INPUTS+1;
	static const int CVMINISTRIP_PARAMS = CVMICROSTRIP_PARAMS;

	cvMiniStrip() : cvMicroStrip()
	{
		recording = false;
	}

	void Create(ModuleWidget *pWidget, float x, float y, int port, int param)
	{
		portID = port;
		paramID = param;

		SvgWidget *pw = createWidget<SvgWidget>(Vec(mm2px(x), yncscape(y, 45.932f)));
		pw->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/cvMiniStrip.svg")));
		pw->wrap();
		pWidget->addChild(pw);

		addMiniStrip(pWidget, x, y, -45.508f);
	}

	protected:
	void addMiniStrip(ModuleWidget *pWidget, float x, float y, float corr)
	{
		pWidget->addInput(createInput<portSmall>(Vec(pos_x(x, 1.072f), pos_y(y, corr + 48.441f, 5.885f)), pWidget->module, CV_IN));
		addMicroStrip(pWidget, x, y, corr);
	}

	public:		
	float TransposeableValue(float v)
	{
		float trnsps = recording ? 0.f : module->inputs[CV_IN].getNormalVoltage(0.0);
		float vmin = getModulableParam(module, PARAM_FROM, RANGE_FROM, LVL_MIN, LVL_MAX);
		float vmax = getModulableParam(module, PARAM_TO, RANGE_TO, LVL_MIN, LVL_MAX);
		float vnt = rescale(v, 0.0, 1.0, std::min(vmin, vmax), std::max(vmin, vmax));
		return clamp(vnt + trnsps, LVL_MIN, LVL_MAX);
	}
	
	protected:
	bool recording = false;
};

// W=0.4 H=3.6 in = 10.160 x 91.440 mm
struct cvStrip : cvMiniStrip
{
	private:
	const float NO_SAMPLE = -1000.f;
	struct cv7segm : TransparentWidget
	{
		private:
		std::shared_ptr<Font> font;
		cvStrip *pStrip;

		public:
		cv7segm()
		{
			pStrip = NULL;
			font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
		}

		void setModule(cvStrip *strp)
		{
			box.size = mm2px(Vec(6.f, 4.4f));
			pStrip = strp;
		}

		void draw(const DrawArgs &args) override
		{
			// Background
			NVGcolor backgroundColor = nvgRGB(0x10, 0x10, 0x10);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
			nvgFillColor(args.vg, backgroundColor);
			nvgFill(args.vg);
			// text
			nvgFontSize(args.vg, 9);
			nvgFontFaceId(args.vg, font->handle);
			nvgTextLetterSpacing(args.vg, 1.1);

			Vec textPos = Vec(3, 10);
			//Vec textPos = Vec(3, 17);
			NVGcolor textColor = nvgRGB(0xdf, 0x2c, 0x2c);
			nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
			nvgText(args.vg, textPos.x, textPos.y, "~~", NULL);

			textColor = nvgRGB(0xff, 0x09, 0x09);
			nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
			nvgText(args.vg, textPos.x, textPos.y, "\\\\", NULL);

			if(pStrip != NULL)
			{
				char n[20];
				sprintf(n, "%2i", pStrip->curStep + 1);
				textColor = nvgRGB(0xff, 0xff, 0xff);
				nvgFillColor(args.vg, textColor);
				nvgText(args.vg, textPos.x, textPos.y, n, NULL);
			}
		}
	};

	public:
	static const int CVSTRIP_INPUTS = CVMINISTRIP_INPUTS+1;
	static const int CVSTRIP_PARAMS = CVMINISTRIP_PARAMS+4;

	cvStrip() : cvMiniStrip()
	{
		maxStep = 0;
		curStep = 0;
		recStep = 0;
		rec_sample = NO_SAMPLE;
	}

	void Init(SequencerWidget *pWidget)
	{
		if(pWidget != NULL)
			pWidget->SetValue(PARAM_REC, 0);
	}

	void Create(ModuleWidget *pWidget, float x, float y, int port, int param, int max_steps)
	{
		portID = port;
		paramID = param;
		maxStep = max_steps;

		SvgWidget *pw = createWidget<SvgWidget>(Vec(mm2px(x), yncscape(y, 91.440f)));
		pw->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/cvStrip.svg")));
		pw->wrap();
		pWidget->addChild(pw);

		addMiniStrip(pWidget, x, y, 0.f);
		pWidget->addInput(createInput<portSmall>(Vec(pos_x(x, 1.072f), pos_y(y, 1.358f, 5.885f)), pWidget->module, GATE_IN));

		ParamWidget *pwdg = createParam<TL1105HSwRed>(Vec(pos_x(x, 1.130f), pos_y(y, 37.650f, 4.477f)), pWidget->module, PARAM_REC);
		((TL1105HSwRed *)pwdg)->randomizable = false;
		pWidget->addParam(pwdg);

		pwdg = createParam<TL1105Sw>(Vec(pos_x(x, 1.597f), pos_y(y, 10.367f, 6.607f)), pWidget->module, PARAM_MANU);
		((TL1105HSwRed *)pwdg)->randomizable = false;
		pWidget->addParam(pwdg);

		pwdg = createParam<RIGHTSWITCH>(Vec(pos_x(x, 2.375f), pos_y(y, 29.497f, 4.627f)), pWidget->module, PARAM_NEXT);
		pWidget->addParam(pwdg);

		pwdg = createParam<LEFTSWITCH>(Vec(pos_x(x, 2.375f), pos_y(y, 20.264f, 4.627f)), pWidget->module, PARAM_PREV);
		pWidget->addParam(pwdg);

		if(pWidget->module != NULL)
		{
			cv7segm *display = createWidget<cv7segm>(Vec(pos_x(x, 0.854), pos_y(y, 24.891, 4.606)));
			display->setModule(this);
			pWidget->addChild(display);			
		}
	}

	bool IsRecAvailable(float *smpl, int *step_n)
	{
		process();
		if(recording && (rec_sample > NO_SAMPLE))
		{
			*smpl = Reverse(rec_sample);
			*step_n = recStep;
			rec_sample = NO_SAMPLE;
			return true;
		}

		return false;
	}

	private:
	SchmittTrigger2 recTrig;
	dsp::SchmittTrigger gateIn;
	int curStep;
	int recStep;
	int maxStep;
	float rec_sample;
	dsp::SchmittTrigger nxtStp;
	dsp::SchmittTrigger prvStp;

	private:	
	void process_keys()
	{
		if(nxtStp.process(module->params[PARAM_NEXT].getValue()))
		{
			if(++curStep >= maxStep)
				curStep = 0;
		} else if(prvStp.process(module->params[PARAM_PREV].getValue()))
		{
			if(--curStep < 0)
				curStep = maxStep - 1;
		}
	}

	void process()
	{
		int stat = recTrig.process(module->params[PARAM_REC].getValue());
		if(stat == 1)	// transizione off->on
		{
			rec_sample = NO_SAMPLE;
			recording = true;
		} else if(stat == -1)// transizione on->off
		{
			rec_sample = NO_SAMPLE;
			recording = false;
		}

		process_keys();
		if(recording)
		{
			bool manual_mode = isSwitchOn(module, PARAM_MANU);
			if(gateIn.process(module->inputs[GATE_IN].getVoltage()))
			{
				recStep = curStep;
				rec_sample = module->inputs[CV_IN].getNormalVoltage(0.0);
				if(!manual_mode)
				{
					if(++curStep >= maxStep)
						curStep = 0;
				}
			}
		}
	}
};

#undef RANGE_FROM
#undef RANGE_TO
#undef CV_IN
#undef GATE_IN
#undef PARAM_FROM
#undef PARAM_TO
#undef PARAM_REC
#undef PARAM_MANU
#undef PARAM_NEXT