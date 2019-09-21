#pragma once

#include "../include/z8exp_base.hpp"

struct z8expM ;
struct z8expMWidget : SequencerWidget
{
	z8expMWidget(z8expM *module);
};

struct z8expM : z8exp
{
	enum ParamIds
	{
		_BASEPARAM,
		NUM_PARAMS = _BASEPARAM + z8exp::NUM_PARAMS
	};
	enum InputIds
	{
		IN_1,
		_BASENPUT = IN_1+16,
		NUM_INPUTS = _BASENPUT + z8exp::NUM_INPUTS
	};
	enum OutputIds
	{
		OUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		_BASELED,
		NUM_LIGHTS = _BASELED + z8exp::NUM_LIGHTS
	};

	z8expM() : z8exp(_BASEPARAM, _BASENPUT, _BASELED)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParams();
		prevVoltage = -1000;
	}
	void process(const ProcessArgs &args) override;
	virtual void onDisconnected() override
	{
		outputs[OUT].setVoltage(LVL_OFF);
	}
private:
	float prevVoltage;
};

