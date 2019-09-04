#include "../include/Nordschleife.hpp"

struct PurpleLight : GrayModuleLightWidget
{
	PurpleLight()
	{
		addBaseColor(SCHEME_PURPLE);
	}
};

struct CKD6B : app::SvgSwitch
{
	CKD6B()
	{
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKD6B_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKD6B_1.svg")));
	}
};

struct StepSelector : app::SvgSwitch
{
	StepSelector()
	{
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/stepSelector_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/stepSelector_1.svg")));
	}
};

struct nordDisplay : TransparentWidget
{
private:
	std::shared_ptr<Font> font;
	Nordschleife *pStrip;

public:
	nordDisplay()
	{
		pStrip = NULL;
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
	}

	void setModule(Nordschleife *strp)
	{
		box.size = mm2px(Vec(41.f, 41.f));
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
		
		}
	}
};
void Nordschleife::process(const ProcessArgs &args)
{

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
		nordDisplay *display = createWidget<nordDisplay>(Vec(mm2px(185.942), yncscape(82.4f, 41.f)));
		display->setModule(module);
		addChild(display);			

		module->cvs.Create(this, 236.228f, 15.367f, Nordschleife::NUM_INPUTS - cvStrip::CVSTRIP_INPUTS, Nordschleife::NUM_PARAMS - cvStrip::CVSTRIP_PARAMS, NORDSTEPS);
	}
	addInput(createInput<PJ301HPort>(Vec(mm2px(236.274), yncscape(3.936f, 8.255f)), module, Nordschleife::RANDOMIZONE));

}

void NordschleifeWidget::createDataEntry(Nordschleife *module)
{
	float x =234.672f;
	float y =113.052f;

	addParam(createParam<CKD6B>(Vec(mm2px(x), yncscape(y, 6.f)), module, Nordschleife::DATAENTRY_OK));
	addParam(createParam<UPSWITCH>(Vec(mm2px(x+0.686f), yncscape(y+6.704f, 4.115f)), module, Nordschleife::DATAENTRY_UP));
	addParam(createParam<DNSWITCH>(Vec(mm2px(x+0.686f), yncscape(y-4.819f, 4.115f)), module, Nordschleife::DATAENTRY_DOWN));
	addParam(createParam<LEFTSWITCH>(Vec(mm2px(x-4.819f), yncscape(y+0.686f, 4.627f)), module, Nordschleife::DATAENTRY_LEFT));
	addParam(createParam<RIGHTSWITCH>(Vec(mm2px(x+6.704f), yncscape(y+0.686f, 4.627f)), module, Nordschleife::DATAENTRY_RIGHT));
}

void NordschleifeWidget::createCar(Nordschleife *module, int index)
{
	float x = 185.222f;

	x += 10.639f * index;
	addInput(createInput<PJ301YPort>(Vec(mm2px(x), yncscape(52.759f, 8.255f)), module, Nordschleife::CAR_RESET + index));
	addInput(createInput<PJ301RPort>(Vec(mm2px(x), yncscape(41.540f, 8.255f)), module, Nordschleife::CAR_CLOCK + index));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(x), yncscape(30.322f, 8.255f)), module, Nordschleife::CAR_CV + index));
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(x), yncscape(19.103f, 8.255f)), module, Nordschleife::CAR_GATE+index));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(x), yncscape(63.978f, 8.255f)), module, Nordschleife::CAR_LAP+index));
	addParam(createParam<CKD6B>(Vec(mm2px(x + 0.423f), yncscape(1.225f, 6.f)), module, Nordschleife::CAR_SELECT+ index));
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
		addParam(createParam<Davies1900hFixRedKnob>(Vec(mm2px(x), yncscape(y, 9.525f)), module, Nordschleife::VOLTAGE_1+index));
	addParam(createParam<StepSelector>(Vec(mm2px(x), yncscape(y - 2.532f, 1.879f)), module, Nordschleife::STEPSELECT_1 + index));

	addOutput(createOutput<portSmall>(Vec(mm2px(x+13.222), yncscape(y+3.365, 5.885f)), module, Nordschleife::OUT_A + index));
	addOutput(createOutput<portSmall>(Vec(mm2px(x+13.222), yncscape(y-2.532, 5.885f)), module, Nordschleife::OUT_B + index));

	addChild(createLight<SmallLight<GreenLight>>(Vec(mm2px(x + 10.339f), yncscape(y + 7.35f, 2.176f)), module, Nordschleife::LOTUS_LED + index));
	addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(x + 10.339f), yncscape(y + 4.027f, 2.176f)), module, Nordschleife::BRABHAM_LED + index));
	addChild(createLight<SmallLight<RedLight>>  (Vec(mm2px(x + 10.339f), yncscape(y + 0.792f, 2.176f)), module, Nordschleife::FERRARI_LED + index));
	addChild(createLight<SmallLight<PurpleLight>>(Vec(mm2px(x + 10.339f), yncscape(y - 2.532f, 2.176f)), module, Nordschleife::HESKETH_LED +index));
}
