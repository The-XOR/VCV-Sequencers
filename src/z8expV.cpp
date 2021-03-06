#include "../include/z8expV.hpp"

void z8expV::process(const ProcessArgs &args)
{
	float rec_smp;
	int rec_step;
	if(cvs.IsRecAvailable(&rec_smp, &rec_step))
	{
		pWidget->SetValue(z8expV::VOLTAGE_1 + rec_step, rec_smp);
	}

	int curStp = getStep();	
	if(curStp >= 0)
	{
		if(curStp != prevStep)
		{
			outputs[OUT].setVoltage(cvs.TransposeableValue(params[VOLTAGE_1+curStp].value));
			prevStep=curStp;
		}
	} 
}

void z8expV::QuantizePitch()
{
	for(int k = 0; k < 16; k++)
		params[VOLTAGE_1 + k].value = pWidget->quantizePitch(VOLTAGE_1 + k, params[VOLTAGE_1 + k].value, cvs);
}

Menu *z8expVWidget::addContextMenu(Menu *menu)
{
	menu->addChild(new SeqMenuItem<z8expVWidget>("Randomize Pitch", this, RANDOMIZE_PITCH));
	menu->addChild(new SeqMenuItem<z8expVWidget>("Pitch Quantization", this, QUANTIZE_PITCH));
	return menu;
}

void z8expVWidget::onMenu(int action)
{
	switch (action)
	{
		case RANDOMIZE_PITCH: std_randomize(z8expV::VOLTAGE_1, z8expV::VOLTAGE_1 + 16); break;
		case QUANTIZE_PITCH: ((z8expV *)module)->QuantizePitch(); break;
	}
}

z8expVWidget::z8expVWidget(z8expV *module)
{
	CREATE_PANEL(module, this, 20, "res/modules/z8expV.svg");
	
	if(module != NULL)
		module->createWidget(this, true);

	addOutput(createOutput<PJ301GPort>(Vec(mm2px(74.432), yncscape(10.517, 8.255)), module, z8expV::OUT));

	float dist_h=29.347-7.122;
	float dist_v=93.354-70.776;
	for(int r = 0; r < 4; r++)
	{
		for(int c = 0; c < 4; c++)
		{
			int n = c + r * 4;
			int posx= 7.122 + dist_h * c;
			addParam(createParam<Davies1900hFixRedKnob>(Vec(mm2px(posx), yncscape(93.354-dist_v * r, 9.525)), module, z8expV::VOLTAGE_1 + n));
			if(module != NULL)
				addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(10.583+posx), yncscape(100.747-dist_v * r, 2.176)), module, module->ledID(n)));
		}
	}

	if(module != NULL)
		module->cvs.Create(this, 90.089f, 3.048f, z8expV::NUM_INPUTS - cvStrip::CVSTRIP_INPUTS, z8expV::NUM_PARAMS - cvStrip::CVSTRIP_PARAMS, 16);
}

