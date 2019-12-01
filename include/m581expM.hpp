#pragma once

#include "../include/m581exp_base.hpp"

struct m581expM ;
struct m581expMWidget : SequencerWidget
{
	m581expMWidget(m581expM *module);
};

struct m581expM : m581exp
{
	enum ParamIds
	{
		NUM_PARAMS
	};
	enum InputIds
	{
		IN_1,
		_BASENPUT = IN_1+64,
		NUM_INPUTS = _BASENPUT + m581exp::NUM_INPUTS
	};
	enum OutputIds
	{
		OUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		_BASELED,
		NUM_LIGHTS = _BASELED + m581exp::NUM_LIGHTS
	};

	m581expM() : m581exp(_BASENPUT, _BASELED)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
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

