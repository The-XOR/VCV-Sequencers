#pragma once

#include "../include/m581exp_base.hpp"

struct m581expV ;
struct m581expVWidget : SequencerWidget
{
	m581expVWidget(m581expV *module);
void onMenu(int action);
private:
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH,
		QUANTIZE_PITCH
	};

	Menu *addContextMenu(Menu *menu) override;
};

struct m581expV : m581exp
{
	enum ParamIds
	{
		VOLTAGE_1,
		_BASEPARAM = VOLTAGE_1 + 64,
		RANGE ,
		NUM_PARAMS = RANGE + cvStrip::CVSTRIP_PARAMS
	};
	enum InputIds
	{
		IN_1,
		_BASENPUT,
		RANGE_IN = _BASENPUT + m581exp::NUM_INPUTS,
		NUM_INPUTS = RANGE_IN + cvStrip::CVSTRIP_INPUTS
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

	m581expV() : m581exp(_BASENPUT, _BASELED)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		cvs.configure(this, NUM_PARAMS - cvStrip::CVSTRIP_PARAMS);		
		for(int n = 0; n < 64; n++)
		{
			configParam(VOLTAGE_1 + n, 0.0, 1.0, 0.0, "Voltage", "V");
		}
	}
	void QuantizePitch();
	void process(const ProcessArgs &args) override;
	virtual void onCreateWidget() override 	{cvs.Init(pWidget); }
	cvStrip cvs;
	virtual void onDisconnected() override
	{
		outputs[OUT].setVoltage(LVL_OFF);
	}
};

