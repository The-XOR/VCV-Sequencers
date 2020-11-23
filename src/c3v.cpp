#include "../include/c3v.hpp"

void c3v::process(const ProcessArgs &args)
{
	float v = 0;
	for (int k = 0; k < C3VCNV; k++)
	{
		if (isLevelOn(this, IN_1 + k))
			v += params[CV_1 + k].value;
	}

	outputs[OUT_1].setVoltage(v);
}


c3vWidget::c3vWidget(c3v *module)
{
	CREATE_PANEL(module, this, 5, "res/modules/c3v.svg");
	float y = 116.504f;
	float yinc = 10.099f;
	for(int k = 0; k < C3VCNV; k++)
	{
		addInput(createInput<portSmall>(Vec(mm2px(1.492), yncscape(y-k*yinc, 5.885)), module, c3v::IN_1+k));
		addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(8.704), yncscape(115.446-k*yinc, 8)), module, c3v::CV_1+k));
	}
	addOutput(createOutput<portGSmall>(Vec(mm2px(18.023), yncscape(61.214, 5.885)), module, c3v::OUT_1));
}
