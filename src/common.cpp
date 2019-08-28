#include "../include/common.hpp"


float SequencerWidget::quantizePitch(int idx, float value, const cvMiniStrip &cvs)
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
