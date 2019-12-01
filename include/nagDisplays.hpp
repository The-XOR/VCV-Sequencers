#pragma once
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
			if (pSeq->enabled)
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
		} else if (pNag != NULL)
		{
			char n[20];
			sprintf(n, "%2i", pNag->degPerClock());
			textColor = nvgRGB(0xff, 0x00, 0x00);
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, textPos.x, textPos.y, n, NULL);
		}
	}
};

struct nagDisplay : OpenGlWidget
{
	nagDisplay()
	{
		pmodule = NULL;
	}

	void SetModule(nag *module)
	{
		pmodule = module;
	}

	void drawFramebuffer() override
	{
		if (pmodule == NULL)
			return;

		NVGcolor dg = SCHEME_LIGHT_GRAY;
		float width = 1.6;
		glViewport(0.0, 0.0, fbSize.x, fbSize.y);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f); //90 degree around x axis
		glOrtho(-1.0, 1, -1.0, 1, -1.0, 1.0);

		// orologio
		//glLineWidth(2*width);
		glColor3f(0, 0, 0);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(0, 0);

		for (int i = 0; i <= NUM_STEPS; i++)
		{
			float angle = fctr1  * i;
			float x = cos(angle);
			float y = sin(angle);
			glVertex2f(x, y);
		}
		glEnd();

		// lancettona
		glLineWidth(width);
		float angle = fctr1 * pmodule->getClock();

		glBegin(GL_LINES);
		glColor3f(dg.r, dg.g, dg.b); //colore lancetta
		glVertex2f(0, 0);
		glVertex2f(cos(angle), sin(angle));
		glEnd();

		for (int k = 0; k < NUM_NAGS; k++)
		{
			NagSeq *pseq = &(pmodule->sequencer[k]);
			if (pseq->enabled)
			{
				if (pseq->Highlight(0))
				{
					glLineWidth(1.3 * width);
					glColor3f(pseq->mycolor.r, pseq->mycolor.g, pseq->mycolor.b);
				}
				else
				{
					glLineWidth(width);
					//glColor3f(dg.r, dg.g, dg.b);
					glColor3f(pseq->mycolor.r *0.8, pseq->mycolor.g *0.8, pseq->mycolor.b *0.8);
				}

				if (pseq->numVertici == 1)
				{
					glBegin(GL_LINES);
					glVertex3f(0, 0, 0);
					float rad = pseq->sequence[0] * fctr2;
					glVertex3f(cos(rad), sin(rad), 0);
				}
				else
				{
					glBegin(GL_LINE_LOOP);
					for (int j = 0; j < pseq->numVertici; j++)
					{
						float rad = pseq->sequence[j] * fctr2;
						glVertex3f(cos(rad), sin(rad), 0);
					}
				}
				glEnd();
			}
		}
	}

private:
	nag *pmodule;
	static constexpr float fctr1 = 2.f * M_PI / NUM_STEPS;
	static constexpr float fctr2 = M_PI / 180.f;
};
#pragma once
