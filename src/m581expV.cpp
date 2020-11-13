#include "../include/m581expV.hpp"

void m581expV::process(const ProcessArgs &args)
{
	float rec_smp;
	int rec_step;
	if(cvs.IsRecAvailable(&rec_smp, &rec_step))
	{
		pWidget->SetValue(m581expV::VOLTAGE_1 + rec_step, rec_smp);
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

void m581expV::QuantizePitch()
{
	for(int k = 0; k < 16; k++)
		params[VOLTAGE_1 + k].value = pWidget->quantizePitch(VOLTAGE_1 + k, params[VOLTAGE_1 + k].value, cvs);
}

Menu *m581expVWidget::addContextMenu(Menu *menu)
{
	menu->addChild(new SeqMenuItem<m581expVWidget>("Randomize Pitch", this, RANDOMIZE_PITCH));
	menu->addChild(new SeqMenuItem<m581expVWidget>("Pitch Quantization", this, QUANTIZE_PITCH));
	return menu;
}

void m581expVWidget::onMenu(int action)
{
	switch (action)
	{
		case RANDOMIZE_PITCH: std_randomize(m581expV::VOLTAGE_1, m581expV::VOLTAGE_1 + 16); break;
		case QUANTIZE_PITCH: ((m581expV *)module)->QuantizePitch(); break;
	}
}

m581expVWidget::m581expVWidget(m581expV *module)
{
	CREATE_PANEL(module, this, 21, "res/modules/m581expV.svg");

	if(module != NULL)
		module->createWidget(this);

	float dist_h = 14.52 - 3.559;
	float dist_v = 97.737 - 86.980;
	float y = 23.016;
	float dled = 10.064 - 3.559;
	float dledY = 102.571 - 97.737;
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(79.626), yncscape(8.930, 8.255)), module, m581expV::OUT));
	for(int c = 0; c < 8; c++)
	{
		for(int r = 0; r < 8; r++)
		{
			int n = c * 8 + r;
			int posx = 3.559 + dist_h * c;
			addParam(createParam<daviesVerySmallRed>(Vec(mm2px(posx), yncscape(y + dist_v * r, 6)), module, m581expV::VOLTAGE_1 + n));
			if (module != NULL)
				addChild(createLight<TinyLight<RedLight>>(Vec(mm2px(dled + posx), yncscape(y + dledY + dist_v * r, 1.088)), module, module->ledID(n)));
		}
	}

	if(module != NULL)
		module->cvs.Create(this, 94.086f, 18.530f, m581expV::NUM_INPUTS - cvStrip::CVSTRIP_INPUTS, m581expV::NUM_PARAMS - cvStrip::CVSTRIP_PARAMS, 64);
}

