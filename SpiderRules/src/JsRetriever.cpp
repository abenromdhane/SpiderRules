#include "jsRetriever.h"
#include "jsapi.h"

JsRetriever::JsRetriever(JSRuntime * runtime, JSClass* classp )
{
	m_JSContext = JS_NewContext(runtime, 8192);		
	
	m_global = new JS::RootedObject(m_JSContext, JS_NewGlobalObject(m_JSContext, classp, nullptr));
	//JSAutoCompartment ac(m_JSContext, *m_global); 
	//JS_InitStandardClasses(m_JSContext, *m_global);
}

JsRetriever::JsRetriever()
{
	m_JSContext = NULL;	
}


void JsRetriever::printJSObject(JS::MutableHandleValue vp)
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

void JsRetriever::parseJSON(const std::wstring json)
{
	JS::RootedValue val(m_JSContext);
	if (JS_ParseJSON(m_JSContext, json.c_str(), json.length(), &val))
	{
		printJSObject(&val);
	}
}

void JsRetriever::init()
{
	ac = new JSAutoCompartment(m_JSContext, *m_global);
	JS_InitStandardClasses(m_JSContext, *m_global);		
}

bool JsRetriever::EvaluateBooleanExpression(std::string expression, std::string ruleName)
{
	int lineno = 0;
	JS::Value rval;
	bool ok = JS_EvaluateScript(m_JSContext, *m_global, expression.c_str(), expression.length(), ruleName.c_str(), lineno, &rval);
	if (ok && !rval.isNull() && rval.isBoolean())
	{
		return rval.toBoolean();
	}
}

jsval JsRetriever::evaluateScript(std::string script, std::string name)
{
	int lineno = 0;
	JS::Value rval;
	JS_EvaluateScript(m_JSContext, *m_global, script.c_str(), script.length(), name.c_str(), lineno, &rval);
	return rval;
}

void JsRetriever::defineValue(std::string name, jsval val)
{
	JS_DefineProperty(m_JSContext, *m_global, name.c_str(),val,NULL,NULL,0);
}

void JsRetriever::defineDoubleValue(std::string name, double defaultValue)
{
	JS_DefineProperty(m_JSContext, *m_global, name.c_str(),JS_NumberValue(defaultValue),NULL,NULL,0);
}

void JsRetriever::defineObjectFromJSON(std::wstring json)
{
	JS::RootedValue rval(m_JSContext);
	JS_ParseJSON(m_JSContext, json.c_str(), json.length(),&rval);
	if (rval.isObject())
	{
		JSObject * jsobj = &rval.toObject();
		JS::AutoIdArray ida(m_JSContext , JS_Enumerate(m_JSContext, jsobj));
		int count = ida.length();
		for (uint32_t i = 0 ; i<count; i++)
		{
			JS::Value element;
			JS_IdToValue(m_JSContext,ida[i], &element);
			if (element.isString())
			{
				std::string elementName = JS_EncodeStringToUTF8(m_JSContext,element.toString());
				jsval vpElement;
				if (JS_GetPropertyById(m_JSContext,jsobj, ida[i], &vpElement))
				{
					JS_DefineProperty(m_JSContext, *m_global, elementName.c_str(), vpElement, NULL, NULL, 0);
				}
			}
		}
	}
}

double JsRetriever::getDoubleValue(std::string valueName)
{
	jsval *vp = new jsval();
	JS_GetProperty(m_JSContext, *m_global, valueName.c_str(), vp);
	if (vp->isNumber())
	{
		return vp->toNumber();
	}
}

JSContext * JsRetriever::getContext()
{
	return m_JSContext;
}

JsRetriever::~JsRetriever()
{
	delete ac;
	delete m_global;
	JS_DestroyContext(m_JSContext);
}