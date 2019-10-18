#include "../include/m581expC.hpp"

void m581expC::process(const ProcessArgs &args)
{
	int curStp = getStep();
	if(curStp >= 0)
	{
		float v = inputs[IN].getNormalVoltage(LVL_ON);
		if(curStp != prevStep || v != prevVoltage)
		{
			if(prevStep >= 0)
				outputs[OUT_1 + prevStep].setVoltage(LVL_OFF);

			outputs[OUT_1+curStp].setVoltage(v);
			prevStep=curStp;
			prevVoltage = v;
		}
	}
}

m581expCWidget::m581expCWidget(m581expC *module)
{
	CREATE_PANEL(module, this, 25, "res/modules/m581expC.svg");

	if(module != NULL)
		module->createWidget(this);

	float dist_h=15;
	float dist_v=11;
	float y = 22;
	addInput(createInput<PJ301BPort>(Vec(mm2px(7.757+7*dist_h), yncscape(9.882, 8.255)), module, m581expC::IN));
	for(int c = 0; c < 8; c++)
	{
		for(int r = 0; r < 8; r++)
		{
			int n = c *8+ r;
			int posx= 7.757 + dist_h * c;
			addOutput(createOutput<PJ301WPort>(Vec(mm2px(posx), yncscape(y+dist_v * r, 9.525)), module, m581expC::OUT_1 + n));
			if(module != NULL)
				addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(8.583+posx), yncscape(y+6.758+dist_v * r, 2.176)), module, module->ledID(n)));
		}
	}
}

