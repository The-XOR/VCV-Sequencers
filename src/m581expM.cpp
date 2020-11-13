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
	CREATE_PANEL(module, this, 18, "res/modules/m581expM.svg");

	if(module != NULL)
		module->createWidget(this);

	float dist_h = 14.52 - 3.559;
	float dist_v = 97.737 - 86.980;
	float y = 23.016;
	float dled = 10.064 - 3.559;
	float dledY = 102.571 - 97.737;
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(79.626), yncscape(8.930, 8.255)), module, m581expM::OUT));
	for(int c = 0; c < 8; c++)
	{
		for(int r = 0; r < 8; r++)
		{
			int n = c *8+ r;
			int posx= 3.559 + dist_h * c;
			addInput(createInput<portSmall>(Vec(mm2px(posx), yncscape(y+dist_v * r, 5.885)), module, m581expM::IN_1 + n));
			if(module != NULL)
				addChild(createLight<TinyLight<RedLight>>(Vec(mm2px(dled + posx), yncscape(y + dledY + dist_v * r, 1.088)), module, module->ledID(n)));
		}
	}
}

