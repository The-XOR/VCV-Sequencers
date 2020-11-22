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
		} else if(inputs[COUNTR_IN].isConnected())
			counter_f = (int)getModulableParam(this, COUNTR_IN_ATT, COUNTR_IN, CCLK_MINVALUE, CCLK_MAXVALUE);

		if (btnup2.process(params[COUNTER_INC2].value))
		{
			int n = roundf(autoreset_f);
			if (n < CCLK_MAXVALUE)
			{
				autoreset_f = n+1;
			}
		} else if (btndwn2.process(params[COUNTER_DEC2].value))
		{
			int n = roundf(autoreset_f);
			if (n > CCLK_MINVALUE)
			{
				autoreset_f = n-1;
			}
		} else if(inputs[RESET_IN].isConnected())
			autoreset_f = (int)getModulableParam(this, RESET_IN_ATT, RESET_IN, CCLK_MINVALUE, CCLK_MAXVALUE);
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

	addInput(createInput<PJ301BPort>(Vec(mm2px(1.783), yncscape(104.342, 8.255)), module, cclk::IN_1));
	addParam(createParam<UPSWITCH>(Vec(mm2px(12.732), yncscape(108.812, 4.115)), module, cclk::COUNTER_INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(12.732), yncscape(104.012, 4.115)), module, cclk::COUNTER_DEC));
	SigDisplayWidget *displayCtr = new SigDisplayWidget(2, 0);
	displayCtr->box.size = Vec(30, 21);
	displayCtr->box.pos = Vec(mm2px(18.385), yncscape(104.294, px2mm(displayCtr->box.size.y)));
	if(module != NULL)
		displayCtr->value = &module->counter_f;
	addChild(displayCtr);
	addInput(createInput<PJ301BPort>(Vec(mm2px(10.160), yncscape(88.080, 8.255)), module, cclk::COUNTR_IN));
	addParam(createParam<Davies1900hFixBlackKnobSmall>(Vec(mm2px(20.256), yncscape(88.207, 8.0)), module, cclk::COUNTR_IN_ATT));

	addInput(createInput<PJ301YPort>(Vec(mm2px(1.783), yncscape(71.150, 8.255)), module, cclk::RESET));
	addParam(createParam<UPSWITCH>(Vec(mm2px(12.732), yncscape(75.620, 4.115)), module, cclk::COUNTER_INC2));
	addParam(createParam<DNSWITCH>(Vec(mm2px(12.732), yncscape(70.820, 4.115)), module, cclk::COUNTER_DEC2));
	displayCtr = new SigDisplayWidget(2, 0);
	displayCtr->box.size = Vec(30, 21);
	displayCtr->box.pos = Vec(mm2px(18.385), yncscape(71.102, px2mm(displayCtr->box.size.y)));
	if (module != NULL)
		displayCtr->value = &module->autoreset_f;
	addChild(displayCtr);
	addInput(createInput<PJ301BPort>(Vec(mm2px(10.160), yncscape(55.007, 8.255)), module, cclk::RESET_IN));
	addParam(createParam<Davies1900hFixBlackKnobSmall>(Vec(mm2px(20.256), yncscape(55.134, 8.0)), module, cclk::RESET_IN_ATT));

	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(2.842), yncscape(25.364, 8.255)), module, cclk::OUT_1));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(19.193), yncscape(25.364, 8.255)), module, cclk::OUT_2));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(2.842), yncscape(7.901, 8.255)), module, cclk::OUT_3));
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(19.193), yncscape(7.901, 8.255)), module, cclk::OUT_4));
}
