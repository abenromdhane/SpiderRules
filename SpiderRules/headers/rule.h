/*
The MIT License (MIT)

Copyright (c) 2014 Ahmed Ben Romdhane

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once
#include<string>
#include "jsRetriever.h"

class Rule
{
private :
	std::string m_name;
	std::string m_condition;
	std::string m_action;
	

public :
	Rule()
	{
	}
	Rule(std::string name, std::string condition, std::string action)
	{
		m_name = name;
		m_condition = condition;
		m_action = action;
	}

	std::string getName()
	{
		return m_name;
	}

	std::string getCondition()
	{
		return m_condition;
	}

	std::string getAction()
	{
		return m_action;
	}

	const bool executeCondition(JsRetriever * jsRetriever)
	{
		return jsRetriever->EvaluateBooleanExpression(m_condition, m_name);
	}
};