#pragma once

#include "../include/m581exp_base.hpp"

struct m581expT ;
struct m581expTWidget : SequencerWidget
{
	m581expTWidget(m581expT *module);
};

struct m581expT : m581exp
{
	enum ParamIds
	{
		Z8PULSE_TIME,
		NUM_PARAMS
	};
	enum InputIds
	{
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

	m581expT() : m581exp(_BASENPUT, _BASELED)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(Z8PULSE_TIME, 10, 2000, 100, "Pulse Time", "mSec");
	}
	void process(const ProcessArgs &args) override;
	virtual void onDisconnected() override
	{
		for(int k = 0; k < 64; k++)
		{
			pulses[k].reset();
			outputs[OUT_1 + k].setVoltage(LVL_OFF);
		}
	}

private:
	PulseGenerator2 pulses[64];
};

