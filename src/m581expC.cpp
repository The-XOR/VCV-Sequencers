#include "../include/m581expC.hpp"

void m581expC::process(const ProcessArgs &args)
{
	int curStp = getStep();
	if(curStp >= 0)
	{
		float v = inputs[IN].getNormalVoltage(LVL_ON);
		if(curStp != prevStep || v != prevVoltage)
		{
			if (prevStep >= 0 && !isSwitchOn(this, HOLD))
				outputs[OUT_1 + prevStep].setVoltage(LVL_OFF);

			outputs[OUT_1+curStp].setVoltage(v);
			prevStep=curStp;
			prevVoltage = v;
		}
	}
}

m581expCWidget::m581expCWidget(m581expC *module)
{
	CREATE_PANEL(module, this, 18, "res/modules/m581expC.svg");

	if(module != NULL)
		module->createWidget(this);

	float dist_h=14.52-3.559;
	float dist_v=97.737-86.980;
	float y = 23.016;
	float dled = 10.064 - 3.559;
	float dledY = 102.571 - 97.737;
	addInput(createInput<PJ301BPort>(Vec(mm2px(79.626), yncscape(8.930, 8.255)), module, m581expC::IN));
	addParam(createParam<TL1105HSw>(Vec(mm2px(42.417), yncscape(10.819, 4.477)), module, m581expC::HOLD));
	for (int c = 0; c < 8; c++)
	{
		for(int r = 0; r < 8; r++)
		{
			int n = c *8+ r;
			int posx= 3.559 + dist_h * c;
			addOutput(createOutput<portWSmall>(Vec(mm2px(posx), yncscape(y+dist_v * r, 5.885)), module, m581expC::OUT_1 + n));
			if(module != NULL)
				addChild(createLight<TinyLight<RedLight>>(Vec(mm2px(dled+posx), yncscape(y+dledY+dist_v * r, 1.088)), module, module->ledID(n)));
		}
	}
}

