#include "../include/m581expT.hpp"

void m581expT::process(const ProcessArgs &args)
{
	int curStp = getStep();
	if(curStp >= 0)
	{
		float v = params[Z8PULSE_TIME].getValue();
		if(curStp != prevStep)
		{
			pulses[curStp].trigger(v/1000.f);
			prevStep=curStp;
		}
	}

	float deltaTime = 1.0 / args.sampleRate;
	// 1 = in process; -1 = terminato; 0 = no operation
	for(int k = 0; k < 16; k++)
	{
		int c = pulses[k].process(deltaTime);
		if(c == 1)
			outputs[OUT_1 + k].setVoltage(LVL_ON);
		else if(c == -1)
			outputs[OUT_1 + k].setVoltage(LVL_OFF);
	}
}

m581expTWidget::m581expTWidget(m581expT *module)
{
	CREATE_PANEL(module, this, 25, "res/modules/m581expT.svg");

	if(module != NULL)
		module->createWidget(this);

	addParam(createParam<Davies1900hFixRedKnob>(Vec(mm2px(73.979), yncscape(9.882, 9.525)), module, m581expT::Z8PULSE_TIME));

	float dist_h=15;
	float dist_v=11;
	float y = 22;
	for(int c = 0; c < 8; c++)
	{
		for(int r = 0; r < 8; r++)
		{
			int n = c *8+ r;
			int posx= 7.757 + dist_h * c;
			addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(posx), yncscape(y+dist_v * r, 9.525)), module, m581expT::OUT_1 + n));
			if(module != NULL)
				addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(8.583+posx), yncscape(y+6.758+dist_v * r, 2.176)), module, module->ledID(n)));
		}
	}
}

