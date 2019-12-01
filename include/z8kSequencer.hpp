#pragma once

struct Z8K;
struct z8kSequencer
{
public:
	void Init(Input *pRst, Input *pDir, Input *pClk, Output *pOut, Light *pLights, std::vector<Param> &params, std::vector<int> steps)
	{
		curStep = 0;
		pReset = pRst;
		pDirection = pDir;
		pClock = pClk;
		pOutput = pOut;
		numSteps = steps.size();
		for(int k = 0; k < numSteps; k++)
		{
			sequence.push_back(&params[steps[k]]);
			leds.push_back(&pLights[steps[k]]);
			chain.push_back(steps[k]);
		}
	}

	void SetSequence(std::vector<Param> &params, Light *pLights, std::vector<int> steps)
	{
		sequence.clear();
		leds.clear();
		chain.clear();
		for(int k = 0; k < numSteps; k++)
		{
			sequence.push_back(&params[steps[k]]);
			leds.push_back(&pLights[steps[k]]);
			chain.push_back(steps[k]);
		}
	}

	inline void Reset()
	{
		curStep = 0;
		leds[0]->value = LED_ON;
		for(int k = 1; k < numSteps; k++)
			leds[k]->value =LED_OFF;
	}

	int Step(Z8K *pModule);

	z8kSequencer()
	{
		pReset = NULL;
		pDirection = NULL;
		pClock = NULL;
		pOutput = NULL;
	}

private:
	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	Input *pReset;
	Input *pDirection;
	Input *pClock;
	Output *pOutput;
	std::vector<Param *> sequence;
	std::vector<Light *> leds;
	std::vector<int> chain;
	int curStep;
	int numSteps;
};

struct Z8K7Segm : TransparentWidget
{
private:
	std::shared_ptr<Font> font;
	Z8K *p8;
	void init(float x, float y)
	{
		box.size = Vec(27, 22);
		box.pos = Vec(mm2px(x), yncscape(y, px2mm(box.size.y)));
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
	}

public:
	Z8K7Segm(Z8K *p, float x, float y)
	{
		p8 = p;
		init(x, y);
	}
	virtual void draw(const DrawArgs &args) override;
};

