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
	void randomize() override
	{
		// do NOT randomaiz
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
		//font = APP->window->loadFont(asset::system("res/fonts/DejaVuSans.ttf"));
	}

	void setModule(Nordschleife *ns)
	{
		box.size = mm2px(Vec(41.f, 105.408f));
		pNord = ns;
	}

	inline int getCurField() {return curField;}

	void moveField(int code)
	{
		switch(code)
		{
			case GLFW_KEY_KP_SUBTRACT:
				if(--curField < 0)
					curField = NORDFIELDS - 1;
				break;

			case GLFW_KEY_KP_ADD:
				if(++curField >= NORDFIELDS)
					curField = 0;
				break;
		}
	}

	private:
	struct drawContext
	{
		float left;
		float top;
		float interleave;
		float interleaveBig;
		float charWidth;
		float charWidthBig;
		NVGcontext *vg;
		
		drawContext(NVGcontext *nvg, std::shared_ptr<Font> &fnt)
		{
			vg = nvg;
			nvgFontFaceId(vg, fnt->handle);		
			// inizializzaziun del contesto
	
			left = 4;
			
			float ascender, descender, lineh;
			setFont(true);
			nvgTextMetrics(vg, &ascender, &descender, &lineh);
			interleaveBig = descender + lineh;
			charWidthBig = nvgText(vg, 0, 0, " ", NULL);

			setFont();
			nvgTextMetrics(vg, &ascender, &descender, &lineh);
			interleave = descender + lineh;
			top = interleave-1;
			charWidth = nvgText(vg, 0, 0, " ", NULL);
		}

		inline void setFont(bool big=false) const	{nvgFontSize(vg, big ? 14 : 10); }
	};

	std::shared_ptr<Font> font;
	Nordschleife *pNord;
	int curField;

	void drawField(const drawContext &ctx, NordschleifeField *pField, bool asCurrent)
	{
		NVGcolor lblColor = nvgRGB(0xff, 0xff, 0xff);
		NVGcolor textColor = pNord->GangBang() ? nvgRGB(0xff, 0xff, 0x00) : nvgRGB(0xff, 0xff, 0xff);
		NVGcolor textCurrentColor = nvgRGB(0x00, 0x00, 0x00);

		float y;
		if(pField->bigField)
		{
			ctx.setFont(true);
			y = ctx.top + ctx.interleaveBig * pField->pos_y-2;
		} else
		{
			ctx.setFont();
			y = ctx.top + ctx.interleave * pField->pos_y;
		}
		y += 2;//margin
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
	
	void draw_info(drawContext &ctx)
	{
		ctx.setFont(true);
		float ascender, descender, lineh;
		nvgTextMetrics(ctx.vg, &ascender, &descender, &lineh);
		ctx.top = descender + lineh;

		if(pNord->GangBang())
		{
			nvgFillColor(ctx.vg, nvgRGB(0xff, 0xff, 0x00));
			nvgText(ctx.vg, ctx.left, ctx.top, "SAFETY CAR", NULL);
		} else
		{
			nvgFillColor(ctx.vg, nvgRGB(0xff, 0xff, 0xff));
			nvgText(ctx.vg, ctx.left, ctx.top, pNord->cars[pNord->selectedCar].name.c_str(), NULL);
		}
		ctx.top += 6;
		ctx.setFont();
		nvgText(ctx.vg, ctx.left, box.size.y-1, pNord->cars[pNord->selectedCar].getLap().c_str(), NULL);
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
	
		drawContext ctx(args.vg, font);
		
		draw_info(ctx);		
		for(int k = 0; k < NORDFIELDS; k++)
			drawField(ctx, &pNord->nsFields[k], !pNord->moveByStep() && k == curField);
	}
};
