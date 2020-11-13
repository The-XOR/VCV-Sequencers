#pragma once

#include "common.hpp"
#include "../include/M581.hpp"

using namespace rack;
extern Plugin *pluginInstance;

#define M581EXP_INPUT			(inputID)
#define M581EXP_LEDINPUT		(lightID)
#define M581EXP_LED_1			(lightID+1)

struct m581exp : Module
{
public:
	void createWidget(SequencerWidget *pwdg) 
	{ 
		pWidget = pwdg;

		pWidget->addInput(createInput<PJ301EXP>(Vec(mm2px(3.559), yncscape(8.93, 8.255)), this, M581EXP_INPUT));
		pWidget->addChild(createLight<TinyLight<WhiteLight>>(Vec(mm2px(3.559 - 2.047), yncscape(8.93 + 7.176, 1.088)), this, M581EXP_LEDINPUT));
		onCreateWidget();
	}
	int ledID(int n) { return n + M581EXP_LED_1; }

protected:
	static const int NUM_INPUTS = 1;
	static const int NUM_LIGHTS = 64+1;

	virtual void onCreateWidget() {}
	virtual void onDisconnected() = 0;

	m581exp(int iid, int lid) : Module()
	{
		inputID=iid;
		lightID=lid;
		pWidget = NULL;
		prevStep = -1;
	}
	
	int getStep()
	{
		if(prevStep >= 0)
			lights[M581EXP_LED_1 + prevStep].value = LED_OFF;

		int rv;
		float expander_out;
		if(IsExpansion(this, &expander_out, EXPPORT_M581, M581EXP_INPUT, M581EXP_LEDINPUT))
		{
			rv = get(expander_out);
			if(rv >=0)
				lights[M581EXP_LED_1+rv].value = LED_ON;

		} else
		{
			if(prevStep != -1)
			{
				prevStep = -1;
				for(int k = 0; k < 64; k++)
					lights[M581EXP_LED_1+ k].value = LED_OFF;
			}
			onDisconnected();

			rv = -1;
		}
		return rv;
	}

	SequencerWidget *pWidget;
	int prevStep;

private:
	inline int get(float expander_out)
	{
		uint8_t *p = (uint8_t *)& expander_out;
		int cur_step = *(p+1);
		int sub_div = *(p+2);
		
		return cur_step * 8 + sub_div;
	}

	int inputID;
	int lightID;
};

#undef M581EXP_INPUT		
#undef M581EXP_LEDINPUT	
#undef M581EXP_LED_1