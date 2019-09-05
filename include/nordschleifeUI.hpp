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
		curField = 0;
		//font = APP->window->loadFont(asset::system("res/fonts/ShareTechMono-Regular.ttf"));
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/FreeMonoBold.ttf"));
	}

	void setModule(Nordschleife *ns)
	{
		box.size = mm2px(Vec(41.f, 41.f));
		pNord = ns;
	}

	inline int getCurField() {return curField;}

	void moveField(int code)
	{
		switch(code)
		{
			case GLFW_KEY_KP_DIVIDE:
				if(--curField < 0)
					curField = NORDFIELDS - 1;
				break;

			case GLFW_KEY_KP_MULTIPLY:
				if(++curField >= NORDFIELDS)
					curField = 0;
				break;
		}
	}

	private:
	struct drawData
	{
		float left;
		float top;
		float interleave;
		float aveCharWidth;
		NVGcontext *vg;
	};

	std::shared_ptr<Font> font;
	Nordschleife *pNord;
	int curField;

	void drawField(const drawData &ctx, NordschleifeField *pField, bool asCurrent)
	{
		NVGcolor lblColor = nvgRGB(0xff, 0xff, 0xff);
		NVGcolor textColor = nvgRGB(0xff, 0xff, 0xff);
		NVGcolor textCurrentColor = nvgRGB(0x00, 0x00, 0x00);

		float y = ctx.top +ctx.interleave * pField->pos_y;
		float x= ctx.left + pField->pos_x;

		nvgFillColor(ctx.vg, lblColor);
		
		x = nvgText(ctx.vg, x, y, pField->label.c_str(), NULL);
		
		std::string txt = pField->getText();
		if(asCurrent)
		{
			float bounds[4];
			nvgTextBounds(ctx.vg, x,  y, txt.c_str(), NULL, bounds);
			nvgBeginPath(ctx.vg);
			nvgFillColor(ctx.vg, textColor);
			nvgRect(ctx.vg, bounds[0], bounds[1], bounds[2] - bounds[0], bounds[3] - bounds[1]);
			nvgFill(ctx.vg);
			nvgFillColor(ctx.vg, textCurrentColor);
		} else
			nvgFillColor(ctx.vg, textColor);

		nvgText(ctx.vg, x, y, txt.c_str(), NULL);
	}
	
	void draw_info(drawData &ctx)
	{
		nvgFontSize(ctx.vg, 16);
		float ascender, descender, lineh;
		nvgTextMetrics(ctx.vg, &ascender, &descender, &lineh);
		ctx.top = descender + lineh + 1/*margine*/;

		nvgFillColor(ctx.vg, nvgRGB(0xff, 0x00, 0x00));
		nvgText(ctx.vg, 0, ctx.top, pNord->cars[pNord->selectedCar].name.c_str(), NULL);
	}

	void draw(const DrawArgs &args) override
	{
		// Background
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
		nvgFillColor(args.vg, nvgRGB(0x00, 0x00, 0x00));
		nvgFill(args.vg);

		if(pNord == NULL)
			return;

		// inizializzaziun del contesto
		nvgFontFaceId(args.vg, font->handle);		
	
		drawData context;
		context.left = 1;
		context.vg = args.vg;
		draw_info(context);

		nvgFontSize(args.vg, 8);
		float ascender, descender, lineh;
		nvgTextMetrics(args.vg, &ascender, &descender, &lineh);
		context.interleave = descender + lineh;
		context.top += context.interleave;

		for(int k = 0; k < NORDFIELDS; k++)
			drawField(context, &pNord->nsFields[k], !pNord->moveByStep() && k == curField);
	}
};
