#pragma once

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
public:
	nordDisplay()
	{
		pNord = NULL;
		font = APP->window->loadFont(asset::system("res/fonts/DejaVuSans.ttf"));
	}

	void setModule(Nordschleife *ns)
	{
		box.size = mm2px(Vec(41.f, 41.f));
		pNord = ns;
	}

private:
	struct drawData
	{
		float left;
		float top;
		float interleave;
		NVGcontext *vg;
	};
	std::shared_ptr<Font> font;
	Nordschleife *pNord;
	
private:


	/*
		void draw_info(const DrawArgs &args)
	{
		NVGcolor textColor = nvgRGB(0xff, 0xff, 0xff);

		nvgTextAlign(args.vg, NVG_ALIGN_LEFT);
		nvgFontSize(args.vg, 9);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, -0.5);
		float ascender, descender, lineh;
		nvgTextMetrics(args.vg, &ascender, &descender, &lineh);
		float interleave = descender + lineh + 2;
		float y = interleave+1;
		float x = 2;
		nvgFillColor(args.vg, textColor);
		char s[100];
		sprintf(s, "Bank %s / #%i", module->GetBankName(), module->GetCurSampleNum()+1);
		nvgText(args.vg, x, y, s, NULL);
		y +=interleave;
		nvgText(args.vg, x, y, module->GetSample().name.c_str(), NULL);
	}
*/
	void draw(const DrawArgs &args) override
	{
		// Background
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
		nvgFillColor(args.vg, nvgRGB(0x00, 0x00, 0x00));
		nvgFill(args.vg);
		if(pNord == NULL)
			return;

		// inizializzaziun del contest
		nvgFontSize(args.vg, 10);
		nvgFontFaceId(args.vg, font->handle);
		//nvgTextLetterSpacing(args.vg, -0.5);
		float ascender, descender, lineh;
		nvgTextMetrics(args.vg, &ascender, &descender, &lineh);
		drawData context;
		context.interleave = descender + lineh + 2;
		context.top = 2;
		context.left = 2;
		context.vg = args.vg;
		nvgFillColor(args.vg, nvgRGB(0,0x90,0));

		/*
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

		*/
	}
};
