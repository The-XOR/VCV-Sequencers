#pragma once

#include "../include/z8exp_base.hpp"

#define MATRIX_SIZE (4)
struct z8expX ;
struct z8expXWidget : SequencerWidget
{
	z8expXWidget(z8expX *module);
};

struct z8expX : z8exp
{
	enum ParamIds
	{
		HOLD,
		NUM_PARAMS
	};
	enum InputIds
	{
		IN_1,
		_BASEINPUT = IN_1+MATRIX_SIZE,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + MATRIX_SIZE
	};
	enum LightIds
	{
		_BASELED,
		NUM_LIGHTS = _BASELED + MATRIX_SIZE * MATRIX_SIZE
	};

	z8expX() : z8exp(0, _BASEINPUT, _BASELED)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParams();
		prevVoltage = -1000;
	}
	void process(const ProcessArgs &args) override;
	virtual void onDisconnected() override
	{
		prevVoltage = -1000;
		for (int k = 0; k < MATRIX_SIZE; k++)
			outputs[OUT_1 + k].setVoltage(LVL_OFF);
	}

private:
	float prevVoltage;
};

