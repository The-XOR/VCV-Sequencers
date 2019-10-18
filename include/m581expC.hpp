#pragma once

#include "../include/m581exp_base.hpp"

struct m581expC ;
struct m581expCWidget : SequencerWidget
{
	m581expCWidget(m581expC *module);
};

struct m581expC : m581exp
{
	enum ParamIds
	{
		NUM_PARAMS
	};
	enum InputIds
	{
		IN,
		_BASENPUT,
		NUM_INPUTS = _BASENPUT + m581exp::NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1+64
	};
	enum LightIds
	{
		_BASELED,
		NUM_LIGHTS = _BASELED + m581exp::NUM_LIGHTS
	};

	m581expC() : m581exp(_BASENPUT, _BASELED)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		prevVoltage = -1000;
	}
	void process(const ProcessArgs &args) override;
	virtual void onDisconnected() override
	{
		prevVoltage = -1000;
		for(int k = 0; k < 64; k++)
		{
			outputs[OUT_1 + k].setVoltage(LVL_OFF);
		}
	}

private:
	float prevVoltage;
};

