#include "../include/cclk.hpp"

void cclk::on_loaded()
{
	load();
}

void cclk::load()
{
	counter = 0;
	reset();
}

void cclk::reset()
{
	counter = 0;
	outputs[OUT_1].setVoltage(LVL_OFF);
	outputs[OUT_2].setVoltage(LVL_OFF);
	outputs[OUT_3].setVoltage(LVL_OFF);
	outputs[OUT_4].setVoltage(LVL_OFF);
}

void cclk::process_keys()
{
	if(pWidget != NULL)
	{
		if(btnup.process(params[COUNTER_INC].value))
		{
			int n = roundf(counter_f);
			if(n < CCLK_MAXVALUE)
			{
				counter_f = n+1;
			}
		} else if(btndwn.process(params[COUNTER_DEC].value))
		{
			int n = roundf(counter_f);
			if(n > CCLK_MINVALUE)
			{
				counter_f = n-1;
			}
		}
		if (btnup2.process(params[COUNTER_INC2].value))
		{
			int n = roundf(autoreset_f);
			if (n < CCLK_MAXVALUE)
			{
				autoreset_f = n+1;
			}
		}
		else if (btndwn2.process(params[COUNTER_DEC2].value))
		{
			int n = roundf(autoreset_f);
			if (n > CCLK_MINVALUE)
			{
				autoreset_f = n-1;
			}
		}
	}
}

void cclk::process(const ProcessArgs &args)
{
	process_keys();

	if(resetTrigger.process(inputs[RESET].getVoltage()))
	{
		reset();

	} else 
	{
		int ar = roundf(autoreset_f);
		if (counterTigger.process(inputs[IN_1].getVoltage()))
		{
			++counter;
			if(ar > 0 && counter == ar)
				reset();
		}
	}

	float v = inputs[IN_1].getVoltage();
	int c = roundf(counter_f);
	if (c > 0)
	{
		bool gt = counter > c;
		int modu = counter % c;

		outputs[OUT_1].setVoltage(modu == 0 ? v : LVL_OFF);
		outputs[OUT_2].setVoltage(modu != 0 ? v : LVL_OFF);
		outputs[OUT_3].setVoltage(!gt ? v : LVL_OFF);
		outputs[OUT_4].setVoltage(gt ? v : LVL_OFF);
	} else
	{
		outputs[OUT_1].setVoltage(v);
		outputs[OUT_2].setVoltage( v);
		outputs[OUT_3].setVoltage(v);
		outputs[OUT_4].setVoltage(v );	
	}
}

cclkWidget::cclkWidget(cclk *module) : SequencerWidget()
{
	if(module != NULL)
		module->setWidget(this);

	CREATE_PANEL(module, this, 6, "res/modules/cclk.svg");

	addParam(createParam<UPSWITCH>(Vec(mm2px(12.732), yncscape(105.108, 4.115)), module, cclk::COUNTER_INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(12.732), yncscape(100.308, 4.115)), module, cclk::COUNTER_DEC));
	addParam(createParam<UPSWITCH>(Vec(mm2px(12.732), yncscape(85.674, 4.115)), module, cclk::COUNTER_INC2));
	addParam(createParam<DNSWITCH>(Vec(mm2px(12.732), yncscape(80.874, 4.115)), module, cclk::COUNTER_DEC2));

	SigDisplayWidget *displayCtr = new SigDisplayWidget(2, 0);
	displayCtr->box.size = Vec(30, 21);
	displayCtr->box.pos = Vec(mm2px(18.385), yncscape(100.59, px2mm(displayCtr->box.size.y)));
	if(module != NULL)
		displayCtr->value = &module->counter_f;
	addChild(displayCtr);

	displayCtr = new SigDisplayWidget(2, 0);
	displayCtr->box.size = Vec(30, 21);
	displayCtr->box.pos = Vec(mm2px(18.385), yncscape(81.156, px2mm(displayCtr->box.size.y)));
	if (module != NULL)
		displayCtr->value = &module->autoreset_f;
	addChild(displayCtr);

	addInput(createInput<PJ301BPort>(Vec(mm2px(1.783), yncscape(100.638, 8.255)), module, cclk::IN_1));
	addInput(createInput<PJ301YPort>(Vec(mm2px(1.783), yncscape(81.204, 8.255)), module, cclk::RESET));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(2.842), yncscape(25.364, 8.255)), module, cclk::OUT_1));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(19.193), yncscape(25.364, 8.255)), module, cclk::OUT_2));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(2.842), yncscape(7.901, 8.255)), module, cclk::OUT_3));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(19.193), yncscape(7.901, 8.255)), module, cclk::OUT_4));
}
