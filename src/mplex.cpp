#include "../include/common.hpp"
#include "../include/mplex.hpp"

void Mplex::on_loaded()
{
	load();
}

void Mplex::load()
{
	set_output(0);
}

void Mplex::set_output(int n)
{
	cur_sel = n;
	for(int k = 0; k < NUM_MPLEX_INPUTS; k++)
	{
		lights[LED_1 + k].value = k == cur_sel ? LVL_ON : LVL_OFF;
	}
}

void Mplex::process_keys()
{
	if(outInc.process(params[OUTPUT_INC].value))
	{
		int n = roundf(num_inputs_f);
		if(n < NUM_MPLEX_INPUTS)
		{
			n += 1;
			num_inputs_f = n;
		}
	} else if(outDec.process(params[OUTPUT_DEC].value))
	{
		int n = roundf(num_inputs_f);
		if(n > 1)
		{
			n -= 1;
			num_inputs_f = n;
		}
	}
}

void Mplex::process(const ProcessArgs &args)
{
	process_keys();
	int num_inputs = roundf(num_inputs_f);

	if(reset.process(inputs[RESET].getVoltage()))
	{
		set_output(0);
	} else if(inputs[CV].isConnected())
	{
		cur_sel = clamp((int)(std::numeric_limits<float>::epsilon() + inputs[CV].getNormalVoltage(0.0)), 0, num_inputs - 1);
		set_output(cur_sel);
	} else
	{
		if(random.process(inputs[RANDOM].getVoltage()))
		{
			set_output(getRand(num_inputs_f));
		} else if(upTrigger.process(params[BTDN].value + inputs[INDN].getVoltage()))
		{
			if(++cur_sel >= num_inputs)
				cur_sel = 0;
			set_output(cur_sel);
		} else if(dnTrigger.process(params[BTUP].value + inputs[INUP].getVoltage()))
		{
			if(--cur_sel < 0)
				cur_sel = num_inputs - 1;
			set_output(cur_sel);
		}
	}
	outputs[OUT_1].setVoltage( inputs[IN_1 + cur_sel].getVoltage());
}

MplexWidget::MplexWidget(Mplex *module) : ModuleWidget()
{
	CREATE_PANEL(module, this, 10, "res/modules/mplex.svg");

	addParam(createParam<BefacoPushBig>(Vec(mm2px(25.322), yncscape(85.436, 8.999)), module, Mplex::BTUP));
	addParam(createParam<BefacoPushBig>(Vec(mm2px(25.322), yncscape(33.452, 8.999)), module, Mplex::BTDN));
	addInput(createInput<PJ301BPort>(Vec(mm2px(25.694), yncscape(71.230, 8.255)), module, Mplex::INUP));
	addInput(createInput<PJ301BPort>(Vec(mm2px(25.694), yncscape(49.014, 8.255)), module, Mplex::INDN));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(40.045), yncscape(60.122, 8.255)), module, Mplex::OUT_1));

	addInput(createInput<PJ301YPort>(Vec(mm2px(40.045), yncscape(108.243, 8.255)), module, Mplex::RESET));
	addInput(createInput<PJ301BPort>(Vec(mm2px(40.045), yncscape(96.422, 8.255)), module, Mplex::RANDOM));
	addInput(createInput<PJ301BPort>(Vec(mm2px(40.045), yncscape(19.939, 8.255)), module, Mplex::CV));

	addParam(createParam<UPSWITCH>(Vec(mm2px(36.932), yncscape(8.814, 4.115)), module, Mplex::OUTPUT_INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(36.932), yncscape(4.025, 4.115)), module, Mplex::OUTPUT_DEC));

	SigDisplayWidget *display = new SigDisplayWidget(1, 0);
	display->box.size = Vec(15, 22);
	display->box.pos = Vec(mm2px(42.319), yncscape(4.539, px2mm(display->box.size.y)));
	if(module != NULL)
		display->value = &module->num_inputs_f;
	addChild(display);

	float y = 105.068;
	float x = 3.558;
	float led_x = 13.843;
	float y_offs = y - 108.108;
	float delta_y = 92.529 - 105.068;
	for(int k = 0; k < NUM_MPLEX_INPUTS; k++)
	{
		addInput(createInput<PJ301GRPort>(Vec(mm2px(x), yncscape(y, 8.255)), module, Mplex::IN_1 + k));
		addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(led_x), yncscape(y - y_offs, 2.176)), module, Mplex::LED_1 + k));
		y += delta_y;
		if(k == 3)
			y -= 2.117;
	}
}
