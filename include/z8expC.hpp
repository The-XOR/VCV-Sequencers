#pragma once

#include "../include/z8exp_base.hpp"

struct z8expC ;
struct z8expCWidget : SequencerWidget
{
	z8expCWidget(z8expC *module);
};

struct z8expC : z8exp
{
	enum ParamIds
	{
		HOLD,
		_BASEPARAM,
		NUM_PARAMS = _BASEPARAM + z8exp::NUM_PARAMS
	};
	enum InputIds
	{
		IN,
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

	z8expC() : z8exp(_BASEPARAM, _BASENPUT, _BASELED)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParams();
		prevVoltage = -1000;
	}
	void process(const ProcessArgs &args) override;
	virtual void onDisconnected() override
	{
		prevVoltage = -1000;
		for(int k = 0; k < 16; k++)
			outputs[OUT_1 + k].setVoltage(LVL_OFF);
	}

private:
	float prevVoltage;
};

