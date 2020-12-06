#pragma once

#define RANGE_FROM		(portID)
#define RANGE_TO		(portID+1)
#define CV_IN			(portID+2)
#define GATE_IN			(portID+3)
#define PARAM_FROM		(paramID+0)
#define PARAM_TO  		(paramID+1)
#define PARAM_QTZ 		(paramID + 2)
#define PARAM_NOTE 		(paramID + 3)
#define __LAST_NOTE 	(PARAM_NOTE + 12)
#define PARAM_REC 		(__LAST_NOTE)
#define PARAM_MANU 		(__LAST_NOTE+1)
#define PARAM_NEXT 		(__LAST_NOTE+2)
#define PARAM_PREV 		(__LAST_NOTE+3)

// 10.160 x 33.728 mm

struct cvMicroStrip
{
	protected:
		static constexpr float CVMICROSTRIP_HEIGHT = 65.853f;
	
	public: 
	static const int CVMICROSTRIP_INPUTS = 2;
	static const int CVMICROSTRIP_PARAMS = 15;

	cvMicroStrip()
	{
	}

	void Create(ModuleWidget *pWidget, float x, float y, int port, int param)
	{
		portID = port;
		paramID = param;

		SvgWidget *pw = createWidget<SvgWidget>(Vec(mm2px(x), yncscape(y, CVMICROSTRIP_HEIGHT)));
		pw->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/cvMicroStrip.svg")));
		pw->wrap();
		pWidget->addChild(pw);

		addMicroStrip(pWidget, x, y, 0);
	}

	void configure(Module *pModule, int param)
	{
		module = pModule;
		paramID = param;
		module->configParam(PARAM_FROM, LVL_MIN, LVL_MAX, -3.f, "Output Range: Min Voltage", "V");
		module->configParam(PARAM_TO, LVL_MIN, LVL_MAX, 3.f, "Output Range: Max Voltage", "V");
		module->configParam(PARAM_QTZ, 0.f, 1.f, 0.f, "Quantize output", "Off/On");
		for (int k = 0; k < 12; k++)
		{
			const char *Notes[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
			module->configParam(PARAM_NOTE + k, 0.f, 1.f, 1.f, Notes[k], "Off/On");
		}
	}

	float Value(float v) const //v normalizzato 0-1
	{
		float vmin = getModulableParam(module, PARAM_FROM, RANGE_FROM, LVL_MIN, LVL_MAX);
		float vmax = getModulableParam(module, PARAM_TO, RANGE_TO, LVL_MIN, LVL_MAX);
		return quantize(clamp(rescale(v, 0.0, 1.0, std::min(vmin, vmax), std::max(vmin, vmax)), LVL_MIN, LVL_MAX));
	}

	float Reverse(float v) const
	{
		float vmin = getModulableParam(module, PARAM_FROM, RANGE_FROM, LVL_MIN, LVL_MAX);
		float vmax = getModulableParam(module, PARAM_TO, RANGE_TO, LVL_MIN, LVL_MAX);
		return clamp(rescale(v, std::min(vmin, vmax), std::max(vmin, vmax), 0.0, 1.0), 0.0, 1.0);
	}

protected:
	float quantize(float v) const
	{
		if(isSwitchOn(module, PARAM_QTZ))
		{
			v = NearestSemitone(v);
			int nota = (int)fabs((round(v - (int)v)/SEMITONE));
			for(int k=0;k<12;k++)
			{
				int offs = (k+nota)%12;
				if(isSwitchOn(module, PARAM_NOTE+offs))
					return v + (offs-nota)*SEMITONE;
			}
		}

		return v;
	}

private:
	struct smallKey : SvgSwitch
	{
		smallKey()
		{
			momentary = false;
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/smallKey_0.svg")));
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/smallKey_1.svg")));
			fb->removeChild(shadow);
		}
	};

protected:
	void addMicroStrip(ModuleWidget *pWidget, float x, float y, float corr)
	{
		pWidget->addInput(createInput<portSmall>(Vec(pos_x(x, 1.072f), pos_y(y, corr + 49.355f, 5.885f)), pWidget->module, RANGE_FROM));
		pWidget->addInput(createInput<portSmall>(Vec(pos_x(x, 1.072f), pos_y(y, corr + 33.356f, 5.885f)), pWidget->module, RANGE_TO));
		pWidget->addParam(createParam<daviesVerySmallBlack>(Vec(pos_x(x, 1.015f), pos_y(y, corr + 56.298f, 6.f)), pWidget->module, PARAM_FROM));
		pWidget->addParam(createParam<daviesVerySmallBlack>(Vec(pos_x(x, 1.015f), pos_y(y, corr + 40.299f, 6.f)), pWidget->module, PARAM_TO));
		pWidget->addParam(createParam<TL1105HSw>(Vec(pos_x(x, 1.755f), pos_y(y, corr + 24.217f, 4.477f)), pWidget->module, PARAM_QTZ));
		for(int k = 0;k<12;k++)		
		{
			float xk;
			if(k == 1 || k ==3 || k==6 || k==8 ||k==10)
				xk = 1.054f;
			else
				xk = 5.563f;
			float yk[12] = {1.757, 3.344, 4.932, 6.519, 8.107, 11.282, 12.869, 14.457, 16.044, 17.632, 19.219, 20.807};
			pWidget->addParam(createParam<smallKey>(Vec(pos_x(x, xk), pos_y(y, corr + yk[k], 2.2f)), pWidget->module, PARAM_NOTE + k));
		}
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
protected:
	static constexpr float CVMINISTRIP_HEIGHT = 76.226f;

public:
	static const int CVMINISTRIP_INPUTS = CVMICROSTRIP_INPUTS+1;
	static const int CVMINISTRIP_PARAMS = CVMICROSTRIP_PARAMS;

	cvMiniStrip() : cvMicroStrip()
	{
		recording = false;
	}

	float TransposeableValue(float v) const
	{
		float trnsps = recording ? 0.f : module->inputs[CV_IN].getNormalVoltage(0.0);
		float vmin = getModulableParam(module, PARAM_FROM, RANGE_FROM, LVL_MIN, LVL_MAX);
		float vmax = getModulableParam(module, PARAM_TO, RANGE_TO, LVL_MIN, LVL_MAX);
		float vnt = rescale(v, 0.0, 1.0, std::min(vmin, vmax), std::max(vmin, vmax));
		return quantize(clamp(vnt + trnsps, LVL_MIN, LVL_MAX));
	}

	void Create(ModuleWidget *pWidget, float x, float y, int port, int param)
	{
		portID = port;
		paramID = param;

		SvgWidget *pw = createWidget<SvgWidget>(Vec(mm2px(x), yncscape(y, CVMINISTRIP_HEIGHT)));
		pw->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/cvMiniStrip.svg")));
		pw->wrap();
		pWidget->addChild(pw);

		addMiniStrip(pWidget, x, y, 0);
	}

	protected:
	void addMiniStrip(ModuleWidget *pWidget, float x, float y, float corr)
	{
		pWidget->addInput(createInput<portSmall>(Vec(pos_x(x, 1.072f), pos_y(y, corr + 2.536f, 5.885f)), pWidget->module, CV_IN));
		addMicroStrip(pWidget, x, y, corr + CVMINISTRIP_HEIGHT-CVMICROSTRIP_HEIGHT);
	}

	bool recording = false;
};

// W=0.4 H=3.6 in = 10.160 x 91.440 mm
struct cvStrip : cvMiniStrip
{
protected:
	static constexpr float CVSTRIP_HEIGHT = 122.403f;

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

		SvgWidget *pw = createWidget<SvgWidget>(Vec(mm2px(x), yncscape(y, CVSTRIP_HEIGHT)));
		pw->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/cvStrip.svg")));
		pw->wrap();
		pWidget->addChild(pw);

		addMiniStrip(pWidget, x, y, CVSTRIP_HEIGHT - CVMINISTRIP_HEIGHT);
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