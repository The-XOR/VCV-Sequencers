#pragma once

#include "../include/z8exp_base.hpp"

struct z8expT ;
struct z8expTWidget : SequencerWidget
{
	z8expTWidget(z8expT *module);
};

struct z8expT : z8exp
{
	enum ParamIds
	{
		Z8PULSE_TIME,
		_BASEPARAM,
		NUM_PARAMS = _BASEPARAM + z8exp::NUM_PARAMS
	};
	enum InputIds
	{
		_BASENPUT,
		NUM_INPUTS = _BASENPUT + z8exp::NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1+16
	};
	enum LightIds
	{
		_BASELED,
		NUM_LIGHTS = _BASELED + z8exp::NUM_LIGHTS
	};

	z8expT() : z8exp(_BASEPARAM, _BASENPUT, _BASELED)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(Z8PULSE_TIME, 10, 2000, 100, "Pulse Time", "mSec");
		configParams();
	}
	void process(const ProcessArgs &args) override;
	virtual void onDisconnected() override
	{
		for(int k = 0; k < 16; k++)
		{
			pulses[k].reset();
			outputs[OUT_1 + k].setVoltage(LVL_OFF);
		}
	}

private:
	PulseGenerator2 pulses[16];
};

