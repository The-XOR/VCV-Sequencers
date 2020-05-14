#include "../include/z8expX.hpp"

void z8expX::process(const ProcessArgs &args)
{
	int curStp = getStep();
	if(curStp >= 0)
	{
	/*	float v = inputs[IN].getNormalVoltage(LVL_ON);
		if(curStp != prevStep || v != prevVoltage)
		{
			if(prevStep >= 0 && !isSwitchOn(this, HOLD))
				outputs[OUT_1 + prevStep].setVoltage(LVL_OFF);

			outputs[OUT_1+curStp].setVoltage(v);
			prevStep=curStp;
			prevVoltage = v;
		}*/
	}
}

z8expXWidget::z8expXWidget(z8expX *module)
{
	CREATE_PANEL(module, this, 18, "res/modules/z8expX.svg");

	if(module != NULL)
		module->createWidget(this);

	addParam(createParam<TL1105HSw>(Vec(mm2px(7.757), yncscape(118.015, 4.477)), module, z8expX::HOLD));

	float x = 22.996;
	float x_led = 26.057;
	float y_led = 89.440;
	float y = 86.378;
	float dx = 40.141 - x;
	float dy = 67.567 - y;
	for (int r = 0; r < MATRIX_SIZE; r++)
	{
		addInput(createInput<PJ301BPort>(Vec(mm2px(7.757), yncscape(y+r*dy, 8.255)), module, z8expX::IN_1+r));
		addOutput(createOutput<PJ301WPort>(Vec(mm2px(x + r * dx), yncscape(10.517, 8.255)), module, z8expX::OUT_1 + r));

		for(int c = 0; c < MATRIX_SIZE; c++)
		{
			int n = c + r * MATRIX_SIZE;
			if(module != NULL)
				addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(x_led+c*dx), yncscape(y_led+r*dy, 2.176)), module, module->ledID(n)));
		}
	}
}

