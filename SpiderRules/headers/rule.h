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
	double returnValue;
	

public :
	Rule();

	Rule(std::string name, std::string condition, std::string action);

	std::string getName();

	std::string getCondition();

	std::string getAction();

	double getReturnValue();

	const bool executeCondition(JsRetriever * jsRetriever);

	void executeAction(JsRetriever * jsRetriever);

};