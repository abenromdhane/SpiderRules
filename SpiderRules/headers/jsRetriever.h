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

#include "jsapi.h"
#include <string>
#include <fstream>
#include <streambuf>

class JsRetriever
{
private : 
	JSContext * m_JSContext;
	JS::RootedObject* m_global;
	JSAutoCompartment* ac;

public:
	JsRetriever(JSRuntime * runtime, JSClass* classp )
	{
		m_JSContext = JS_NewContext(runtime, 8192);		
		
		m_global = new JS::RootedObject(m_JSContext, JS_NewGlobalObject(m_JSContext, classp, nullptr));
		//JSAutoCompartment ac(m_JSContext, *m_global); 
		//JS_InitStandardClasses(m_JSContext, *m_global);
	}

	JsRetriever()
	{
		m_JSContext = NULL;	
	}


	void printJSObject(JS::MutableHandleValue vp)
	{
		if (vp.isObject())
		{
			JSObject * jsobj = &vp.toObject();
			JS::AutoIdArray ida(m_JSContext , JS_Enumerate(m_JSContext, jsobj));
			int count = ida.length();
			for (uint32_t i = 0 ; i<count; i++)
			{
				JS::Value element;
				JS_IdToValue(m_JSContext,ida[i], &element);
				if (element.isString())
				{
					std::string elementName = JS_EncodeStringToUTF8(m_JSContext,element.toString());
					printf("%s : ", elementName.c_str());
					jsval vpElement;
					if (JS_GetPropertyById(m_JSContext,jsobj, ida[i], &vpElement))
					{
						if (vpElement.isNumber())
						{
							double elementValue = vpElement.toNumber();
							printf("%f \n", elementValue);
						}
						if (vpElement.isString())
						{
							std::string elementValue = JS_EncodeStringToUTF8(m_JSContext,vpElement.toString());
							printf("%s \n", elementValue.c_str());
						}
						if (vpElement.isObject())
						{
							JS::RootedValue vObject(m_JSContext,vpElement);
							printJSObject(&vObject);
						}
					}
				}else if (element.isInt32())
				{
					jsval vpElement;
					if (JS_GetPropertyById(m_JSContext,jsobj, ida[i], &vpElement))
					{
						if (vpElement.isNumber())
						{
							double elementValue = vpElement.toNumber();
							printf("%f \n", elementValue);
						}
						if (vpElement.isString())
						{
							std::string elementValue = JS_EncodeStringToUTF8(m_JSContext,vpElement.toString());
							printf("%s \n", elementValue.c_str());
						}
					}
					JS::RootedValue vArray(m_JSContext,vpElement);
					printJSObject(&vArray);
				}
           
			}
		}
	}

	void parseJSON(const std::wstring json)
	{
		JS::RootedValue val(m_JSContext);
		if (JS_ParseJSON(m_JSContext, json.c_str(), json.length(), &val))
		{
			printJSObject(&val);
		}
	}

	void init()
	{
		ac = new JSAutoCompartment(m_JSContext, *m_global);
		JS_InitStandardClasses(m_JSContext, *m_global);		
	}
	bool EvaluateBooleanExpression(std::string expression, std::string ruleName)
	{
		int lineno = 0;
		JS::Value rval;
		bool ok = JS_EvaluateScript(m_JSContext, *m_global, expression.c_str(), expression.length(), ruleName.c_str(), lineno, &rval);
		if (ok && !rval.isNull() && rval.isBoolean())
		{
			return rval.toBoolean();
		}
	}

	jsval evaluateScript(std::string script, std::string name)
	{
		int lineno = 0;
		JS::Value rval;
		JS_EvaluateScript(m_JSContext, *m_global, script.c_str(), script.length(), name.c_str(), lineno, &rval);
		return rval;
	}
	

	JSContext * getContext()
	{
		return m_JSContext;
	}


	~JsRetriever()
	{
		delete ac;
		delete m_global;
		JS_DestroyContext(m_JSContext);
	}
};