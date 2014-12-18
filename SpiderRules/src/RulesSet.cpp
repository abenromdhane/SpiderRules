#include "RulesSet.h"
#include <iostream>

RulesSet::RulesSet(JSClass * classp)
{
	rt = JS_NewRuntime(8L * 1024 * 1024, JS_NO_HELPER_THREADS);
	esprimaRetriever = new JsRetriever(rt, classp);
	esprimaRetriever->init();
	rulesExecutioner = new JsRetriever(rt, classp);
	rulesExecutioner->init();
}

void RulesSet::parseTokenizeObject(unsigned int ruleIndex, JS::MutableHandleValue rootVal)
{
	Rule* rule = rulesVector[ruleIndex];
	if (rootVal.isObject())
	{
		JSObject * jsobj = &rootVal.toObject();
		JS::AutoIdArray ida(esprimaRetriever->getContext() , JS_Enumerate(esprimaRetriever->getContext(), jsobj));
		int count = ida.length();
		for (uint32_t i = 0 ; i<count; i++)
		{
			JS::Value element;
			JS_IdToValue(esprimaRetriever->getContext(),ida[i], &element);
			if (element.isInt32())
			{
				jsval tokenObjectVal;
				if (JS_GetPropertyById(esprimaRetriever->getContext(),jsobj, ida[i], &tokenObjectVal))
				{
					if (tokenObjectVal.isObject())
					{
						jsval nameValue;
						jsval typeValue;
						JS_GetProperty(esprimaRetriever->getContext(), &tokenObjectVal.toObject(), "value", &nameValue);
						JS_GetProperty(esprimaRetriever->getContext(), &tokenObjectVal.toObject(), "type", &typeValue);
						if (nameValue.isString() && typeValue.isString())
						{
							std::string typeString = JS_EncodeString(esprimaRetriever->getContext(), typeValue.toString());
							std::string nameString = JS_EncodeString(esprimaRetriever->getContext(), nameValue.toString());
							if (typeString == "Identifier")
							{
								int i = 0;
								BOOST_FOREACH(Rule* currentRule, rulesVector)
								{										
									if(currentRule->getName() == nameString)
									{
										CustomEdge edge; 
										bool edgeExists;
										boost::tie(edge, edgeExists) = boost::edge(vertices[ruleIndex] , vertices[i], customGraph);
										if(!edgeExists)
											boost::add_edge(vertices[ruleIndex] , vertices[i], customGraph);
									}
									i++;
								}					
							}
						}
					}
				}
			}
		}
	}
}

	

void RulesSet::setInputObject(std::wstring json)
{
	rulesExecutioner->defineObjectFromJSON(json);
}

void RulesSet::createRuleDependencies(unsigned int ruleIndex)
{
	Rule* rule = rulesVector[ruleIndex];
	int lineno = 0;
	JS::Value rval;
	std::ifstream t("esprima.js");
	std::string str((std::istreambuf_iterator<char>(t)),
             std::istreambuf_iterator<char>());
	esprimaRetriever->evaluateScript(str,"esprima.js");
	{
		str = "esprima.tokenize(\""+rule->getCondition() +"\")";
		rval = esprimaRetriever->evaluateScript(str, "");

		JS::RootedValue rootVal(esprimaRetriever->getContext(),rval);
		parseTokenizeObject(ruleIndex, &rootVal);
	}
	{
		str = "esprima.tokenize(\""+rule->getAction() +"\")";
		rval = esprimaRetriever->evaluateScript(str, "");
		
		JS::RootedValue rootVal(esprimaRetriever->getContext(),rval);
		parseTokenizeObject(ruleIndex, &rootVal);
	}
}

void RulesSet::addRule(Rule* rule)
{
	rulesVector.push_back(rule);
	vertices.push_back(boost::add_vertex(rule, customGraph));
	rulesExecutioner->defineDoubleValue(rule->getName(), 0.0);
}

void RulesSet::createGraphDependency()
{
	for(int index = 0; index<rulesVector.size();index++)
	{
		createRuleDependencies(index);
	}
}

void RulesSet::executeRulesSet()
{
	MyVisitor vis;
	vis.jsRetriever = rulesExecutioner;
	boost::depth_first_search(customGraph, boost::visitor(vis));
}

RulesSet::~RulesSet()
{
	delete esprimaRetriever;
	delete rulesExecutioner;
	JS_DestroyRuntime(rt);
	JS_ShutDown();
}

void RulesSet::printRulesResult()
{
	BOOST_FOREACH(Rule * rule, rulesVector)
	{
		printf("%s = %f \n",rule->getName().c_str(), rule->getReturnValue());
	}
}

void RulesSet::bindInputData(std::wstring json)
{
	rulesExecutioner->defineObjectFromJSON(json);
}

void MyVisitor::discover_vertex(CustomVertex v, const CustomGraph& g) const
  {
	bool test = g.m_vertices[v].m_property->executeCondition(jsRetriever);
	if (test)
	{
		g.m_vertices[v].m_property->executeAction(jsRetriever);
	}
    std::cout << v << std::endl;
    return;
  }