#include "Rule.h"


Rule::Rule()
{
}

Rule::Rule(std::string name, std::string condition, std::string action)
{
	m_name = name;
	m_condition = condition;
	m_action = action;
	returnValue = 0.0;
}

std::string Rule::getName()
{
	return m_name;
}

std::string Rule::getCondition()
{
	return m_condition;
}

std::string Rule::getAction()
{
	return m_action;
}

double Rule::getReturnValue()
{
	return returnValue;
}

const bool Rule::executeCondition(JsRetriever * jsRetriever)
{
	return jsRetriever->EvaluateBooleanExpression(m_condition, m_name);
}

void Rule::executeAction(JsRetriever * jsRetriever)
{
	jsval val = jsRetriever->evaluateScript(m_action, m_name);
	jsRetriever->defineValue(m_name, val);
	if (val.isNumber())
	{
		returnValue = val.toNumber();
	}
}