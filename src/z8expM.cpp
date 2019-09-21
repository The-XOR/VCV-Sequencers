#include "../include/z8expM.hpp"

void z8expM::process(const ProcessArgs &args)
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

z8expMWidget::z8expMWidget(z8expM *module)
{
	CREATE_PANEL(module, this, 18, "res/modules/z8expM.svg");

	if(module != NULL)
		module->createWidget(this);

	addOutput(createOutput<PJ301WPort>(Vec(mm2px(74.432), yncscape(10.517, 8.255)), module, z8expM::OUT));

	float dist_h=29.347-7.122;
	float dist_v=93.354-70.776;
	for(int r = 0; r < 4; r++)
	{
		for(int c = 0; c < 4; c++)
		{
			int n = c + r * 4;
			int posx= 7.757 + dist_h * c;
			addInput(createInput<PJ301BPort>(Vec(mm2px(posx), yncscape(93.989-dist_v * r, 9.525)), module, z8expM::IN_1 + n));
			if(module != NULL)
				addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(10.583+posx), yncscape(100.747-dist_v * r, 2.176)), module, module->ledID(n)));
		}
	}
}

