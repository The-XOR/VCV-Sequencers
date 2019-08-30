#include "../include/common.hpp"


float SequencerWidget::quantizePitch(int idx, float value, const cvMicroStrip &cvs)
{
	value = cvs.Value(value);
	value = cvs.Reverse(NearestSemitone(value));
	int index = getParamIndex(idx);
	if(index >= 0)
	{
		params[index]->paramQuantity->setValue(value);
	}

	return value;
}

bool isSwitchOn(Module *pm, int paramId)
{
	return (pm->params[paramId].getValue() > SWITCH_ON);
}

bool getModulableSwitch(Module *pm, int paramId, int inputId) 
{ 
	return (pm->inputs[inputId].getNormalVoltage(0.0) > SWITCH_ON) || isSwitchOn(pm, paramId); 
}

float getModulableParam(Module *pm, int paramId, int inputId, float minValue, float maxValue)
{
	float v = (pm->inputs[inputId].getNormalVoltage(0.0)/LVL_MAX) * maxValue;
	return clamp(v+ pm->params[paramId].getValue(), minValue, maxValue);
}
