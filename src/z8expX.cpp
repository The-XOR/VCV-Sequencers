#include "../include/z8expX.hpp"

void z8expX::process(const ProcessArgs &args)
{
	bool enabled[Z8K::NUM_Z8SEQUENCERS];
	for (int k = 0; k < Z8K::NUM_Z8SEQUENCERS; k++)
		enabled[k] = isSwitchOn(this, ENABLE_1 + k);

	int curStp = getMatrix(enabled);
	drawMatrix(curStp);
	for (int c = 0; c < MATRIX_SIZE; c++)
	{
		float outpval = 0.f;
		for (int r = 0; r < MATRIX_SIZE; r++)
			outpval += patchBay[c][r] * inputs[IN_1+r].getVoltage(0.f);
		outputs[OUT_1+c].setVoltage(outpval);
	}
}

void z8expX::matrixOff()
{
	for (int c = 0; c < MATRIX_SIZE; c++)
		for (int r = 0; r < MATRIX_SIZE; r++)
				patchBay[c][r] = 0.f;
}

void z8expX::drawMatrix(int curStp)
{
	for (int c = 0; c < MATRIX_SIZE; c++)
	{
		int righe_attive = 0;
		for (int r = 0; r < MATRIX_SIZE; r++)
		{
			int n = c + r * MATRIX_SIZE;
			int bitmask = 1 << n;
			bool is_on =  (curStp & bitmask) > 0;
			if(is_on && inputs[IN_1+r].isConnected())
			{
				righe_attive++;
				patchBay[c][r] = 1.f;;
			} else
				patchBay[c][r] = 0.f;
			lights[ledID(n)].value = is_on ? LED_ON : LED_OFF;
		}	
		if(righe_attive >0)
		{
			float coeff = 1.f / righe_attive;
			for (int r = 0; r < MATRIX_SIZE; r++)
				patchBay[c][r] *= coeff;
		}
	}
}

z8expXWidget::z8expXWidget(z8expX * module)
{
	CREATE_PANEL(module, this, 18, "res/modules/z8expX.svg");

	if (module != NULL)
		module->createWidget(this, false);

	float x = 22.996;
	float x_led = 26.057;
	float y_led = 89.440;
	float y = 86.378;
	float dx = 40.141 - x;
	float dy = 67.567 - y;
	for (int r = 0; r < MATRIX_SIZE; r++)
	{
		addInput(createInput<PJ301BPort>(Vec(mm2px(7.757), yncscape(y + r * dy, 8.255)), module, z8expX::IN_1 + r));
		addOutput(createOutput<PJ301WPort>(Vec(mm2px(x + r * dx), yncscape(10.517, 8.255)), module, z8expX::OUT_1 + r));

		for (int c = 0; c < MATRIX_SIZE; c++)
		{
			int n = c + r * MATRIX_SIZE;
			if (module != NULL)
				addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(x_led + c * dx), yncscape(y_led + r * dy, 2.176)), module, module->ledID(n)));
		}
	}

	float ySw = 105.440;
	float xSw[] = { 7.757,
					13.048,
					18.340,
					23.632,
					37.390,
					42.682,
					47.974,
					53.265,
					64.378,
					69.669,
					74.961
	};

	for (int k = 0; k < Z8K::NUM_Z8SEQUENCERS; k++)
	{
		addParam(createParam<TL1105Sw>(Vec(mm2px(xSw[k]), yncscape(ySw, 6.607)), module, z8expX::ENABLE_1 + k));
	}
}

