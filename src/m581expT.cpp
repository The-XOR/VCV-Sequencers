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
	CREATE_PANEL(module, this, 18, "res/modules/m581expT.svg");

	if(module != NULL)
		module->createWidget(this);

	addParam(createParam<Davies1900hFixRedKnobSmall>(Vec(mm2px(79.067), yncscape(9.058, 8.0)), module, m581expT::Z8PULSE_TIME));

	float dist_h = 14.52 - 3.559;
	float dist_v = 97.737 - 86.980;
	float y = 23.016;
	float dled = 10.064 - 3.559;
	float dledY = 102.571 - 97.737;
	for (int c = 0; c < 8; c++)
	{
		for(int r = 0; r < 8; r++)
		{
			int n = c * 8 + r;
			int posx = 3.559 + dist_h * c;
			addOutput(createOutput<portBLUSmall>(Vec(mm2px(posx), yncscape(y + dist_v * r, 5.885)), module, m581expT::OUT_1 + n));
			if(module != NULL)
				addChild(createLight<TinyLight<RedLight>>(Vec(mm2px(dled + posx), yncscape(y + dledY + dist_v * r, 1.088)), module, module->ledID(n)));
		}
	}
}

