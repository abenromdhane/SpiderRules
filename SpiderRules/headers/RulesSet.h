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
#include "rule.h"
#include "jsRetriever.h"
#include <vector>
#include <fstream>
#include <streambuf>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

typedef boost::adjacency_list<boost::vecS,boost::vecS,boost::directedS, Rule*> CustomGraph;
typedef boost::graph_traits<CustomGraph>::vertex_descriptor CustomVertex;
typedef boost::graph_traits<CustomGraph>::edge_descriptor CustomEdge;

class MyVisitor : public boost::default_dfs_visitor
{
public:
  void discover_vertex(CustomVertex v, const CustomGraph& g) const
  {
	bool test = g.m_vertices[v].m_property->executeCondition(jsRetriever);
    std::cout << v << std::endl;
    return;
  }
  JsRetriever* jsRetriever;
};

class RulesSet
{
private :
	JsRetriever* esprimaRetriever;
	JSRuntime *rt;
	JsRetriever* rulesExecutioner;
	std::vector<Rule*> rulesVector;

	void parseTokenizeObject(unsigned int ruleIndex, JS::MutableHandleValue rootVal)
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
public :


	CustomGraph customGraph;
	std::vector<CustomVertex> vertices;
	RulesSet(JSClass * classp)
	{
		rt = JS_NewRuntime(8L * 1024 * 1024, JS_NO_HELPER_THREADS);
		esprimaRetriever = new JsRetriever(rt, classp);
		esprimaRetriever->init();
		rulesExecutioner = new JsRetriever(rt, classp);
		rulesExecutioner->init();
	}


	void createRuleDependencies(unsigned int ruleIndex)
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

	void addRule(Rule* rule)
	{
		rulesVector.push_back(rule);
		vertices.push_back(boost::add_vertex(rule, customGraph));
	}

	void createGraphDependency()
	{
		for(int index = 0; index<rulesVector.size();index++)
		{
			createRuleDependencies(index);
		}
	}

	void executeRulesSet()
	{
		MyVisitor vis;
		vis.jsRetriever = rulesExecutioner;
		boost::depth_first_search(customGraph, boost::visitor(vis));
	}

		~RulesSet()
	{
		delete esprimaRetriever;
		delete rulesExecutioner;
		JS_DestroyRuntime(rt);
		JS_ShutDown();
	}
};