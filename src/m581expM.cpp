#include "../include/m581expM.hpp"

void m581expM::process(const ProcessArgs &args)
{
	int curStp = getStep();
	if(curStp >= 0)
	{
		float v = inputs[IN_1+ curStp].getNormalVoltage(LVL_OFF);
		if(curStp != prevStep || v != prevVoltage)
		{
			outputs[OUT].setVoltage(v);
			prevStep=curStp;
			prevVoltage = v;
		}
	}
}

m581expMWidget::m581expMWidget(m581expM *module)
{
	CREATE_PANEL(module, this, 25, "res/modules/m581expM.svg");

	if(module != NULL)
		module->createWidget(this);

	float dist_h=15;
	float dist_v=11;
	float y = 22;
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(7.757+7*dist_h), yncscape(9.882, 8.255)), module, m581expM::OUT));
	for(int c = 0; c < 8; c++)
	{
		for(int r = 0; r < 8; r++)
		{
			int n = c *8+ r;
			int posx= 7.757 + dist_h * c;
			addInput(createInput<PJ301BPort>(Vec(mm2px(posx), yncscape(y+dist_v * r, 9.525)), module, m581expM::IN_1 + n));
			if(module != NULL)
				addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(8.583+posx), yncscape(y+6.758+dist_v * r, 2.176)), module, module->ledID(n)));
		}
	}
}

