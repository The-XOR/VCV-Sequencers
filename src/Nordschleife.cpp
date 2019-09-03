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
		box.size = mm2px(Vec(41.231, 42.19	));
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
	CREATE_PANEL(module, this, 48, "res/modules/Nordschleife.svg");
	for(int k = 0; k < NORDSTEPS; k++)
		createStep(module, k);
	for(int k = 0; k < NORDCARS; k++)
		createCar(module, k);

	createDataEntry(module);
	
	if(module != NULL)
	{
		nordDisplay *display = createWidget<nordDisplay>(Vec(mm2px(185.702), yncscape(78.914, 28.569)));
		display->setModule(module);
		addChild(display);			

		module->cvs.Create(this, 232.524f, 18.542f, Nordschleife::NUM_INPUTS - cvStrip::CVSTRIP_INPUTS, Nordschleife::NUM_PARAMS - cvStrip::CVSTRIP_PARAMS, NORDSTEPS);
	}
}

void NordschleifeWidget::createDataEntry(Nordschleife *module)
{
	addParam(createParam<CKD6B>(Vec(mm2px(203.317f), yncscape(66.661f, 6.f)), module, Nordschleife::DATAENTRY_OK));
	addParam(createParam<UPSWITCH>(Vec(mm2px(204.003f), yncscape(73.365f, 4.115f)), module, Nordschleife::DATAENTRY_UP));
	addParam(createParam<DNSWITCH>(Vec(mm2px(204.003f), yncscape(61.843f, 4.115f)), module, Nordschleife::DATAENTRY_DOWN));
	addParam(createParam<LEFTSWITCH>(Vec(mm2px(198.498f), yncscape(67.347f, 4.627f)), module, Nordschleife::DATAENTRY_LEFT));
	addParam(createParam<RIGHTSWITCH>(Vec(mm2px(210.021f), yncscape(67.347f, 4.627f)), module, Nordschleife::DATAENTRY_RIGHT));

}

void NordschleifeWidget::createCar(Nordschleife *module, int index)
{
	float x = 185.222f;

	x += 10.639f * index;
	addInput(createInput<PJ301YPort>(Vec(mm2px(x), yncscape(52.759f, 8.255f)), module, Nordschleife::CAR_RESET + index));
	addInput(createInput<PJ301RPort>(Vec(mm2px(x), yncscape(41.540f, 8.255f)), module, Nordschleife::CAR_CLOCK + index));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(x), yncscape(30.322f, 8.255f)), module, Nordschleife::CAR_CV + index));
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(x), yncscape(19.103f, 8.255f)), module, Nordschleife::CAR_GATE+index));
	addParam(createParam<CKD6B>(Vec(mm2px(x + 0.423f), yncscape(1.225f, 6.f)), module, Nordschleife::CAR_SELECT+ index));
}

void NordschleifeWidget::createStep(Nordschleife *module, int index)
{
	float x = 9.423;
	float y = 120.686;
	int c = index % 8;
	int r = index / 8;

	x += 21.767 * c;
	y -= 21.079 * r;
	addOutput(createOutput<portSmall>(Vec(mm2px(x), yncscape(y, 5.885f)), module, Nordschleife::LOTUS_OUT + index));
	addOutput(createOutput<portSmall>(Vec(mm2px(x + 6.611f), yncscape(y - 1.439f, 5.885f)), module, Nordschleife::BRABHAM_OUT + index));
	addOutput(createOutput<portSmall>(Vec(mm2px(x + 10.808f), yncscape(y - 6.385f, 5.885f)), module, Nordschleife::FERRARI_OUT + index));
	addOutput(createOutput<portSmall>(Vec(mm2px(x+11.224f), yncscape(y-12.746f, 5.885f)), module, Nordschleife::HESKETH_OUT+index));

	if(c == 0 || c == 4)
		addParam(createParam<Davies1900hFixBlackKnob>(Vec(mm2px(x + 0.222f), yncscape(y - 10.632f, 9.525f)), module, Nordschleife::VOLTAGE_1 + index));
	else
		addParam(createParam<Davies1900hFixRedKnob>(Vec(mm2px(x+0.222f), yncscape(y -10.632f, 9.525f)), module, Nordschleife::VOLTAGE_1+index));

	addChild(createLight<SmallLight<GreenLight>>(Vec(mm2px(x + 5.342f), yncscape(y + 4.42f, 2.176f)), module, Nordschleife::LOTUS_LED + index));
	addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(x + 11.953f), yncscape(y + 2.981f, 2.176f)), module, Nordschleife::BRABHAM_LED + index));
	addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(x + 16.15f), yncscape(y - 1.965f, 2.176f)), module, Nordschleife::FERRARI_LED + index));
	addChild(createLight<SmallLight<PurpleLight>>(Vec(mm2px(x + 16.566f), yncscape(y - 8.326f, 2.176f)), module, Nordschleife::HESKETH_LED +index));

	addParam(createParam<StepSelector>(Vec(mm2px(x + 0.222f), yncscape(y - 13.164f, 1.879f)), module, Nordschleife::STEPSELECT_1 + index));
}
