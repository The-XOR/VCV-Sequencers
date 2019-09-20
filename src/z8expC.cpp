#include "../include/z8expC.hpp"

void z8expC::process(const ProcessArgs &args)
{
	float expander_out;
	uint8_t *p = (uint8_t *)&expander_out;
	
	if(IsExpansion(this, &expander_out, EXPPORT_Z8K, EXP_IN, EXP_LED))
	{
		float v = inputs[IN].getNormalVoltage(LVL_ON);
		int chn = (int)round(params[CHANNEL].value);
		int curStp = *(p+chn);
		if(curStp != prevStep || v != prevVoltage)
		{
			if(prevStep >=0)
			{
				lights[LED_1+prevStep].value = LED_OFF;
				outputs[OUT_1+prevStep].setVoltage(LVL_OFF);
			}

			outputs[OUT_1+curStp].setVoltage(v);
			lights[LED_1+curStp].value = v;
			prevStep=curStp;
			prevVoltage = v;
		}
	} else if(prevStep != -1)
	{
		prevStep = -1;
		prevVoltage = -1000;
		for(int k = 0; k < 16; k++)
		{
			lights[LED_1+k].value = LED_OFF;
			outputs[OUT_1+k].setVoltage(LVL_OFF);
		}
	}
}

z8expCWidget::z8expCWidget(z8expC *module)
{
	CREATE_PANEL(module, this, 18, "res/modules/z8expC.svg");

	if(module != NULL)
		module->setWidget(this);

	addInput(createInput<PJ301BPort>(Vec(mm2px(45.857), yncscape(10.517, 8.255)), module, z8expC::IN));
	addInput(createInput<PJ301EXP>(Vec(mm2px(7.757), yncscape(10.517, 8.255)), module, z8expC::EXP_IN));
	addChild(createLight<TinyLight<WhiteLight>>(Vec(mm2px(7.757-2.047), yncscape(10.517+7.176, 1.088)), module, z8expC::EXP_LED));
	addParam(createParam<CKSSThreeFix>(Vec(mm2px(23.070), yncscape(9.645, 10.0)), module, z8expC::CHANNEL));

	float dist_h=29.347-7.122;
	float dist_v=93.354-70.776;
	for(int r = 0; r < 4; r++)
	{
		for(int c = 0; c < 4; c++)
		{
			int n = c + r * 4;
			int posx= 7.757 + dist_h * c;
			addOutput(createOutput<PJ301WPort>(Vec(mm2px(posx), yncscape(93.989-dist_v * r, 9.525)), module, z8expC::OUT_1 + n));
			addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(10.583+posx), yncscape(100.747-dist_v * r, 2.176)), module, z8expC::LED_1 +n));
		}
	}
}

