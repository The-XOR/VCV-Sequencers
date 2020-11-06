#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct Burst;
struct BurstWidget : SequencerWidget
{
	BurstWidget(Burst *module);
};

#define NUM_BURST_PORTS (6)
struct Burst : Module
{
	enum ParamIds
	{
		OUT_SPAN,
		EVENT_COUNT,
		MODE,
		MODE_INVERT,
		TRIGGER,
		TRIG_THRESH,
		NUM_PARAMS
	};

	enum InputIds
	{
		CLOCK_IN,
		OUT_SPAN_IN,
		EVENT_COUNT_IN,
		TRIGGER_THRESH_IN,
		RESET,
		NUM_INPUTS
	};

	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_BURST_PORTS
	};

	enum LightIds
	{
		LEDOUT_1,
		NUM_LIGHTS = LEDOUT_1 + NUM_BURST_PORTS
	};

	Burst() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(Burst::MODE, 0.0, 2.0, 0.0);
		configParam(Burst::MODE_INVERT, 0.0, 1.0, 0.0);
		configParam(Burst::OUT_SPAN, 1.0, NUM_BURST_PORTS, 1.0, "Span", "#");
		configParam(Burst::EVENT_COUNT, 0.0, 23.0, 0.0, "Event Counter", "#", 0, 1, 1);
		configParam(Burst::TRIG_THRESH, LVL_MIN, LVL_MAX, LVL_OFF, "Threshold", "V");
		configParam(Burst::TRIGGER, 0.0, 1.0, 0.0);
		onReset();
	}

	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void dataFromJson(json_t *root) override { Module::dataFromJson(root); on_loaded(); }
	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		return rootJ;
	};

private:
	void on_loaded();
	void load();
	void all_off();
	void prepare_step();
	void next_step();
	void end_step();
	void port(int n, bool on) { outputs[OUT_1 + n].setVoltage(lights[LEDOUT_1 + n].value = on ? LVL_MAX : LVL_OFF); }
	void invert_port(int n) { port(n, outputs[OUT_1 + n].value < LVL_MAX); }

private:
	SchmittTrigger2 clock;
	dsp::SchmittTrigger trigger;
	dsp::SchmittTrigger resetTrigger;
	bool active;
	bool trigger_pending;
	enum MODE
	{
		FWD = 0,
		PEND = 1,
		RAND = 2
	};
	struct
	{
		int cycle_counter;
		int max_cycle;
		int out_span;
		int max_span;
		enum MODE mode;
		bool invert_mode;
		bool retrogade;
		bool first_cycle;
	} activating_params;

};
