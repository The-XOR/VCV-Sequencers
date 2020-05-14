#pragma once

#include "common.hpp"
#include "../include/Z8K.hpp"

using namespace rack;
extern Plugin *pluginInstance;

#define Z8EXP_CHANNEL		(paramID)
#define Z8EXP_CHANNELY		(1+paramID)
#define Z8EXP_XYMODE		(2+paramID)
#define Z8EXP_INPUT			(inputID)
#define Z8EXP_LEDINPUT		(lightID)
#define Z8EXP_LED_1			(lightID+1)

struct z8exp : Module
{
public:
	void createWidget(SequencerWidget *pwdg, bool withControls) 
	{ 
		pWidget = pwdg;
		pWidget->addInput(createInput<PJ301EXP>(Vec(mm2px(7.757), yncscape(10.517, 8.255)), this, Z8EXP_INPUT));
		pWidget->addChild(createLight<TinyLight<WhiteLight>>(Vec(mm2px(7.757 - 2.047), yncscape(10.517 + 7.176, 1.088)), this, Z8EXP_LEDINPUT));
		if(withControls)
		{
			Davies1900hFixWhiteKnob *p = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(25.083), yncscape(9.882, 9.525)), this, Z8EXP_CHANNEL);
			p->snap = true;
			pWidget->addParam(p);
			p = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(56.832), yncscape(9.882, 9.525)), this, Z8EXP_CHANNELY);
			p->snap = true;
			pWidget->addParam(p);
			pWidget->addParam(createParam<TL1105HSw>(Vec(mm2px(44.591), yncscape(12.406, 4.477)), this, Z8EXP_XYMODE));
		}
		onCreateWidget();
	}
	int ledID(int n) { return n + Z8EXP_LED_1; }

protected:
	static const int NUM_PARAMS = 3;
	static const int NUM_INPUTS = 1;
	static const int NUM_LIGHTS = 16+1;

	virtual void onCreateWidget() {}
	virtual void onDisconnected() = 0;

	z8exp(int pid, int iid, int lid) : Module()
	{
		paramID=pid;
		inputID=iid;
		lightID=lid;
		pWidget = NULL;
		prevStep = -1;
	}
	
	void configParams()
	{
		configParam(Z8EXP_CHANNEL, 0, Z8K::SequencerIds::NUM_Z8SEQUENCERS - 1, 0.0);
		configParam(Z8EXP_CHANNELY, 0, Z8K::SequencerIds::NUM_Z8SEQUENCERS - 1, 0.0);
	}

	int getStep()
	{
		if (prevStep >= 0)
			lights[Z8EXP_LED_1 + prevStep].value = LED_OFF;

		int rv;
		float expander_out;
		if(IsExpansion(this, &expander_out, EXPPORT_Z8K, Z8EXP_INPUT, Z8EXP_LEDINPUT))
		{
			rv = isSwitchOn(this, Z8EXP_XYMODE) ? getXY(expander_out) : get(expander_out, (int)round(params[Z8EXP_CHANNEL].value));
			if (rv >= 0)
				lights[Z8EXP_LED_1 + rv].value = LED_ON;
		} else
		{
			if (prevStep != -1)
			{
				prevStep = -1;
				for(int k = 0; k < 16; k++)
					lights[Z8EXP_LED_1+ k].value = LED_OFF;
			}

			onDisconnected();

			rv = -1;
		}
		return rv;
	}

	SequencerWidget *pWidget;
	int prevStep;

	int getMatrix(bool enabled[Z8K::NUM_Z8SEQUENCERS])
	{
		int rv = 0;
		float expander_out;
		if (IsExpansion(this, &expander_out, EXPPORT_Z8K, Z8EXP_INPUT, Z8EXP_LEDINPUT))
		{
			rv = 0;
			for (int k = 0; k < Z8K::NUM_Z8SEQUENCERS; k++)
			{
				if (enabled[k])
					rv |= (1 << get(expander_out, k));
			}
		} else
		{
			onDisconnected();
		}

		return rv;
	}

private:
	int get(float expander_out, int chn)
	{
		uint8_t *p = (uint8_t *)& expander_out;
		switch(chn)
		{
			case Z8K::SEQ_1 + 0: return (*(p + 0)) & 0x3;
			case Z8K::SEQ_1 + 1: return 4 + ((*(p + 0) >> 2) & 0x3);
			case Z8K::SEQ_1 + 2: return 8 + ((*(p + 0) >> 4) & 0x3);
			case Z8K::SEQ_1 + 3: return 12 + ((*(p + 0) >> 6) & 0x3);

			case Z8K::SEQ_A + 0: return 4 * ((*(p + 1)) & 0x3);
			case Z8K::SEQ_A + 1: return 1 + 4 * ((*(p + 1) >> 2) & 0x3);
			case Z8K::SEQ_A + 2: return 2 + 4 * ((*(p + 1) >> 4) & 0x3);
			case Z8K::SEQ_A + 3: return 3 + 4 * ((*(p + 1) >> 6) & 0x3);

			case Z8K::SEQ_HORIZ: return  (*(p + 2)) & 0x0f;
			case Z8K::SEQ_VERT: return  (*(p + 2) >> 4) & 0x0f;
			case Z8K::SEQ_PATH: return (*(p + 3)) & 0x0f;
		}

		return -1;
	}

	int getXY(float expander_out)
	{
		int x = get(expander_out, (int)round(params[Z8EXP_CHANNEL].value));
		int y = get(expander_out, (int)round(params[Z8EXP_CHANNELY].value));
		if(x >= 0 && y >= 0)
		{
			int c = x % 4;
			int r = y / 4;
			return r * 4 + c;
		}

		return -1;
	}
	int paramID;
	int inputID;
	int lightID;
};

#undef Z8EXP_CHANNEL	
#undef Z8EXP_CHANNELY	
#undef Z8EXP_XYMODE	
#undef Z8EXP_INPUT		
#undef Z8EXP_LEDINPUT	
#undef Z8EXP_LED_1