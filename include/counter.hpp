#pragma once
#include "common.hpp"

#define COUNTER_MINVALUE 2
#define COUNTER_MAXVALUE 999

struct Counter;
struct CounterWidget : SequencerWidget
{
	CounterWidget(Counter *module);
	void SetCounter(int n);
};

struct Counter : Module
{
	enum ParamIds
	{
		COUNTER_INC, COUNTER_DEC,
		COUNTER,
		ONESHOT,
		NUM_PARAMS
	};
	enum InputIds
	{
		RESET,
		IN_1,
		IN_COUNTER,
		NUM_INPUTS
	};

	enum OutputIds
	{
		OUT_1,
		OUT_TGL,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		ACTIVE,
		TOGGLESTAT,
		NUM_LIGHTS
	};

	Counter() : Module()
	{
		pWidget = NULL;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(Counter::COUNTER_INC, 0.0, 1.0, 0.0);
		configParam(Counter::COUNTER_DEC, 0.0, 1.0, 0.0);
		configParam(Counter::COUNTER, COUNTER_MINVALUE, COUNTER_MAXVALUE, COUNTER_MINVALUE, "Counter", "#");

		on_loaded();
	}

	void process(const ProcessArgs &args) override;

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_t *bpmJson = json_integer((int)counter_f);
		json_object_set_new(rootJ, "counter", bpmJson);
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *bpmJson = json_object_get(rootJ, "counter");
		if(bpmJson)
			counter_f = (float)json_integer_value(bpmJson);
		on_loaded();
	}

	void onReset() override
	{
		load();
	}

	void onRandomize() override {}
	void setWidget(CounterWidget *pwdg) { pWidget = pwdg; }
	float counter_f;
	float countDown;

private:
	dsp::SchmittTrigger btnup;
	dsp::SchmittTrigger btndwn;
	CounterWidget *pWidget;
	int curCounter;
	bool toggle_status;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger counterTigger;
	dsp::PulseGenerator outPulse;

	void process_keys();

	void on_loaded();
	void load();
	void reset();
	void trig_out();
};
