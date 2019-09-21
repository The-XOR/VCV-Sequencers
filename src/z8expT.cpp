#include "../include/z8expT.hpp"

void z8expT::process(const ProcessArgs &args)
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

z8expTWidget::z8expTWidget(z8expT *module)
{
	CREATE_PANEL(module, this, 18, "res/modules/z8expT.svg");

	if(module != NULL)
		module->createWidget(this);

	addParam(createParam<Davies1900hFixRedKnob>(Vec(mm2px(73.979), yncscape(9.882, 9.525)), module, z8expT::Z8PULSE_TIME));

	float dist_h=29.347-7.122;
	float dist_v=93.354-70.776;
	for(int r = 0; r < 4; r++)
	{
		for(int c = 0; c < 4; c++)
		{
			int n = c + r * 4;
			int posx= 7.757 + dist_h * c;
			addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(posx), yncscape(93.989-dist_v * r, 9.525)), module, z8expT::OUT_1 + n));
			if(module != NULL)
				addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(10.583+posx), yncscape(100.747-dist_v * r, 2.176)), module, module->ledID(n)));
		}
	}
}

