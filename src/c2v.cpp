#include "../include/c2v.hpp"

void c2v::process(const ProcessArgs &args)
{
	for(int k = 0; k < C2VCNV; k++)
	{
		if(outputs[OUT_1+k].isConnected())
			outputs[OUT_1+k].setVoltage(inputs[IN_1+k].getVoltage() > 0 ? params[CV_1+k].value : 0.0);
	}
}


c2vWidget::c2vWidget(c2v *module)
{
	CREATE_PANEL(module, this, 5, "res/modules/c2v.svg");
	float y = 116.504f;
	float yinc = 10.099f;
	for(int k = 0; k < C2VCNV; k++)
	{
		addInput(createInput<portSmall>(Vec(mm2px(1.492), yncscape(y-k*yinc, 5.885)), module, c2v::IN_1+k));
		addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(8.704), yncscape(115.446-k*yinc, 8)), module, c2v::CV_1+k));
		addOutput(createOutput<portGSmall>(Vec(mm2px(18.023), yncscape(y-k*yinc, 5.885)), module, c2v::OUT_1+k));
	}
}

