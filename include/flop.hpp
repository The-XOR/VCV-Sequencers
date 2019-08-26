#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

#define NUM_FLOPS		(4)		
struct flop;
struct flopWidget : ModuleWidget
{
	flopWidget(flop *module);
};

struct flop : Module
{
	enum ParamIds
	{
		THRESH_A,
		THRESH_B = THRESH_A + NUM_FLOPS,
		HIZ = THRESH_B + NUM_FLOPS,
		COMPAREMODE,
		NUM_PARAMS
	};
	enum InputIds
	{
		IN_A,
		IN_B = IN_A + NUM_FLOPS,
		NUM_INPUTS = IN_B + NUM_FLOPS
	};
	enum OutputIds
	{
		OUT_1,
		OUTNEG_1 = OUT_1 + NUM_FLOPS,
		NUM_OUTPUTS = OUTNEG_1 + NUM_FLOPS
	};
	enum LightIds
	{
		LED_A,
		LED_B = LED_A + NUM_FLOPS,
		LED_OUT = LED_B + NUM_FLOPS,
		LED_OUTNEG = LED_OUT + NUM_FLOPS,
		NUM_LIGHTS = LED_OUTNEG + NUM_FLOPS
	};

	flop() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < NUM_FLOPS; k++)
		{
			configParam(flop::THRESH_A + k, LVL_MIN, LVL_MAX, LVL_OFF, "Threshold", "V");
			configParam(flop::THRESH_B + k, LVL_MIN, LVL_MAX, LVL_OFF, "Threshold", "V");
			a_Trig[k].reset();
			b_Trig[k].reset();
			setOutput(k, false);
		}
		jk_state = sr_state = t_state = d_state = false;
		hizvalue = 0;
		hizCounter = 0;
	}
	void process(const ProcessArgs &args) override;

private:
	struct LogicSchmittTrigger
	{
		bool prev_state = false;

		int process(bool in)
		{
			if(in != prev_state)
			{
				prev_state = in;
				return in ? 1 : -1; 
			}

			return 0;
		}

		void reset()
		{
			prev_state = false;
		}
	};

	void process(int num_op, bool hiz, bool compare);
	bool logicLevel(float v1, float v2, bool compare);
	float getVoltage(int index, bool hiz);
	void setOutput(int index, bool on);
	bool flipflip_SR(bool s, bool r);
	bool flipflip_JK(bool j, bool k);
	bool flipflip_T(bool t, bool clk, int trig_b);
	bool flipflip_D(bool d, bool clk, int trig_b);
	bool sr_state;
	bool jk_state;
	bool t_state;
	bool d_state;
	LogicSchmittTrigger a_Trig[NUM_FLOPS];
	LogicSchmittTrigger b_Trig[NUM_FLOPS];
	unsigned int hizCounter;
	float hizvalue;
};
