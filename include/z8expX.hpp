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
		_BASEINPUT,
		IN_1,
		NUM_INPUTS = IN_1 + MATRIX_SIZE
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + MATRIX_SIZE
	};
	enum LightIds
	{
		_BASELED,
		NUM_LIGHTS = _BASELED + z8exp::NUM_LIGHTS
	};

	z8expX() : z8exp(NUM_PARAMS, _BASEINPUT, _BASELED)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs &args) override;
	virtual void onDisconnected() override
	{
		for (int k = 0; k < MATRIX_SIZE; k++)
			outputs[OUT_1 + k].setVoltage(LVL_OFF);
	}

private:
	void drawMatrix(int n);
};

