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
		font = APP->window->loadFont(asset::system("res/fonts/ShareTechMono-Regular.ttf"));
		createFields();
	}

	~nordDisplay()
	{
		for(int k = 0; k < (int)fields.size(); k++)
			delete fields[k];
		fields.clear();
	}

	void setModule(Nordschleife *ns)
	{
		box.size = mm2px(Vec(41.f, 41.f));
		pNord = ns;
	}

	void moveField(int code)
	{
		switch(code)
		{
			case GLFW_KEY_KP_DIVIDE:
				if(--curField < 0)
					curField = (int)fields.size() - 1;
				break;

			case GLFW_KEY_KP_MULTIPLY:
				if(++curField >= (int)fields.size())
					curField = 0;
				break;

			case GLFW_KEY_KP_SUBTRACT:
				fields[curField]->dec();
				break;
			case GLFW_KEY_KP_ADD:
				fields[curField]->inc();
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
	struct _field
	{
		public:
		void inc()
		{
			if(++value > maxValue)
				value = minValue;
		}
		void dec()
		{
			if(--value < minValue)
				value = maxValue;
		}
		void draw(const drawData &ctx, bool asCurrent)
		{
			float y = ctx.interleave * _y;
			float x=lblx * ctx.aveCharWidth;

			nvgFillColor(ctx.vg, lblColor);
			x += 2/*margine*/+nvgText(ctx.vg, x, ctx.top + y, Nordschleife::label[myID], NULL);

			const char *txt = getTxt(value);
			if(asCurrent)
			{
				float bounds[4];
				nvgTextBounds(ctx.vg, x, ctx.top + y, txt, NULL, bounds);
				nvgBeginPath(ctx.vg);
				nvgFillColor(ctx.vg, textColor);
				nvgRect(ctx.vg, bounds[0], bounds[1], bounds[2] - bounds[0], bounds[3] - bounds[1]);
				nvgFill(ctx.vg);
				nvgFillColor(ctx.vg, textCurrentColor);
			} else
				nvgFillColor(ctx.vg, textColor);

			nvgText(ctx.vg, x, ctx.top + y, txt, NULL);
		}

		protected:
		_field(NordschleifeFields fieldID, int posx, int posy)
		{
			myID = fieldID;
			lblx = posx;
			_y = posy;
		}
		virtual const char *getTxt(int v) = 0;
		void setBounds(int mi, int ma)
		{
			minValue = mi;
			maxValue = ma;
		}

		private:
		NVGcolor lblColor = nvgRGB(0xff, 0xff, 0xff);
		NVGcolor textColor = nvgRGB(0xff, 0xff, 0xff);
		NVGcolor textCurrentColor = nvgRGB(0x00, 0x00, 0x00);
		float lblx;
		float _y;
		NordschleifeFields myID;
		int minValue;
		int maxValue;
	};

	struct numField : _field
	{
		numField(NordschleifeFields fieldID, int posx, int posy, int minv, int maxv, int precision = 2) :_field(fieldID, posx, posy)
		{
			setBounds(minv, maxv);
			sprintf(fmt, "%%0%ii", precision);
		}

		private:
		char fmt[20];
		char s_value[20];

		protected:
		virtual const char *getTxt(int v) override
		{
			sprintf(s_value, fmt, v);
			return s_value;
		}
	};

	struct strField : _field
	{
		strField(NordschleifeFields fieldID, int posx, int posy, std::vector<std::string> valueList) :_field(fieldID, posx, posy)
		{
			values = valueList;
			setBounds(0, (int)values.size() - 1);
		}

		private:
		std::vector<std::string> values;

		protected:
		virtual const char *getTxt(int v) override
		{
			return values[v].c_str();
		}
	};

	std::vector<_field *> fields;
	int curField;

	void createFields()
	{
		std::vector<std::string> direz = {"Forward", "Backward", "Alternate", "Brownian", "Random"};

		// la posizione X,Y e' in CARATTERI, con (0,0) = toppo righto
		fields.push_back(new numField(NordschleifeFields::shlfStep, 0, 0, 1, 64));
		fields.push_back(new strField(NordschleifeFields::shlfDirection, 1, 1, direz));
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
		nvgFontSize(args.vg, 9);
		nvgFontFaceId(args.vg, font->handle);		
		float ascender, descender, lineh;
		float bounds[4];
		nvgTextMetrics(args.vg, &ascender, &descender, &lineh);
		nvgTextBounds(args.vg, 0, 0, "a", NULL, bounds);

		drawData context;
		context.aveCharWidth= bounds[2] - bounds[0];
		context.interleave = descender + lineh;
		context.top = context.interleave + 2/*margine*/;
		context.left = 2;
		context.vg = args.vg;

		for(int k = 0; k < (int)fields.size(); k++)
			fields[k]->draw(context, !pNord->moveByStep() && k == curField);
	}
};
