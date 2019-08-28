#include "../include/Spiralone.hpp"
#include "../include/sprlnSequencer.hpp"
#include "../include/SpiraloneModule.hpp"

extern float AccessParam(Spiralone *p, int seq, int id);
extern float AccessParam(Spiralone *p, int id);
extern Input *AccessInput(Spiralone *p, int seq, int id);
extern float *AccessOutput(Spiralone *p, int seq, int id);
extern float *AccessLight(Spiralone *p, int id);

int spiraloneSequencer::GetNumSteps(Spiralone *pSpir) { return getInput(pSpir, Spiralone::INLENGHT_1, Spiralone::LENGHT_1, 1.0, TOTAL_STEPS); }

void spiraloneSequencer::Step(Spiralone *pSpir)
{
	if(resetTrigger.process(AccessInput(pSpir, seq, Spiralone::RESET_1)->value))
		Reset(pSpir);
	else
	{
		int clk = clockTrig.process(AccessInput(pSpir, seq, Spiralone::CLOCK_1)->value); // 1=rise, -1=fall
		if(clk == 1)
		{
			int mode = (int)std::roundf(AccessParam(pSpir, seq, Spiralone::MODE_1));
			int numSteps = GetNumSteps(pSpir);
			int stride = getInput(pSpir, Spiralone::INSTRIDE_1, Spiralone::STRIDE_1, 1.0, 8.0);

			*AccessLight(pSpir, ledID()) = LED_OFF;
			switch(mode)
			{
			case 0: // fwd:
				curPos += stride;
				break;

			case 2: // bwd
				curPos -= stride;
				break;

			case 1: //a gradire
				if(random::uniform() > 0.5)
					curPos += stride;
				else
					curPos -= stride;
			}
			if(curPos < 0)
				curPos = numSteps + curPos;

			curPos %= numSteps;

			outputVoltage(pSpir);
			gate(clk, pSpir);
		} else if(clk == -1)
			gate(clk, pSpir);
	}
}

void spiraloneSequencer::Reset(Spiralone *pSpir)
{
	curPos = 0;
	for(int k = 0; k < TOTAL_STEPS; k++)
		*AccessLight(pSpir, ledID(k)) = LED_OFF;
}

int spiraloneSequencer::getInput(Spiralone *pSpir, int input_id, int knob_id, float minValue, float maxValue)
{
	float getNormalVoltaged_in = AccessInput(pSpir, seq, input_id)->isConnected() ? rescale(AccessInput(pSpir, seq, input_id)->value, LVL_OFF, LVL_ON, 0.0, maxValue) : 0.0;
	float v = clamp(getNormalVoltaged_in + AccessParam(pSpir, seq, knob_id), minValue, maxValue);
	return (int)roundf(v);
}

void spiraloneSequencer::outputVoltage(Spiralone *pSpir)
{
	float v =  AccessInput(pSpir, seq, Spiralone::INXPOSE_1)->getNormalVoltage(0.0);
	v += AccessParam(pSpir, Spiralone::VOLTAGE_1 + curPos);
	*AccessOutput(pSpir, seq, Spiralone::CV_1) = pSpir->cvs.TransposeableValue(v)+AccessParam(pSpir, seq, Spiralone::XPOSE_1);
}

void spiraloneSequencer::gate(int clk, Spiralone *pSpir)
{
	if(clk == 1)
	{
		*AccessLight(pSpir, ledID()) = LED_ON;
		*AccessOutput(pSpir, seq, Spiralone::GATE_1) = LVL_ON;
	} else if(clk == -1) // fall
	{
		*AccessOutput(pSpir, seq, Spiralone::GATE_1) = LVL_OFF;
	}
}
