#include "../include/volt.hpp"

struct volt7Segm : TransparentWidget
{
private:
	std::shared_ptr<Font> font;
	volt *pvolt;
	void init(float x, float y)
	{
		box.size = Vec(40, 22);
		box.pos = Vec(mm2px(x), yncscape(y, px2mm(box.size.y)));
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
	}

public:
	volt7Segm(volt *p, float x, float y)
	{
		pvolt = p;
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

		if(pvolt != NULL)
		{
			char n[20];
			sprintf(n, "%3i", pvolt->divider * 12 );
			textColor = nvgRGB(0xff, 0x00, 0x00);
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, textPos.x, textPos.y, n, NULL);
		}
	}
};
void volt::process(const ProcessArgs &args)
{
	process_keys();
}

void volt::process_keys()
{
	if(btninc.process(params[INC].value))
	{
		if(divider < DIVIDER_MAX)
			divider++;
	} else if(btndec.process(params[DEC].value))
	{
		if(divider > DIVIDER_MIN)
			divider--;
	}
	if(outputs[OUT_1].isConnected())
	{
		float v = inputs[IN_1].getNormalVoltage(0.0);
		float approx = 1.0f / (12.0f * divider);
		outputs[OUT_1].setVoltage( round(v / approx) * approx);
	}
}

voltWidget::voltWidget(volt *module)
{
	CREATE_PANEL(module, this, 4, "res/modules/volt.svg");
	addInput(createInput<PJ301BPort>(Vec(mm2px(6.032), yncscape(25.494, 8.255)), module, volt::IN_1));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(6.032), yncscape(8.372, 8.255)), module, volt::OUT_1));
	addParam(createParam<UPSWITCH>(Vec(mm2px(5.533), yncscape(108.637, 4.115)), module, volt::INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(10.160), yncscape(108.637, 4.115)), module, volt::DEC));
	addChild(new volt7Segm(module != NULL ? module : NULL, 5.427, 99.5));
}

