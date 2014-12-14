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
#include <vector>
#include <fstream>
#include <streambuf>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>


class RulesDependencies
{
private :
	JSContext *esprimaContext;
	JS::RootedObject* esprimaGlobal;
	JSAutoCompartment* esprimaCompartement;

	void parseTokenizeObject(std::vector<Rule> ruleSet, unsigned int ruleIndex, JS::MutableHandleValue rootVal)
	{
		Rule rule = ruleSet[ruleIndex];
		if (rootVal.isObject())
		{
			JSObject * jsobj = &rootVal.toObject();
			JS::AutoIdArray ida(esprimaContext , JS_Enumerate(esprimaContext, jsobj));
			int count = ida.length();
			for (uint32_t i = 0 ; i<count; i++)
			{
				JS::Value element;
				JS_IdToValue(esprimaContext,ida[i], &element);
				if (element.isInt32())
				{
					jsval tokenObjectVal;
					if (JS_GetPropertyById(esprimaContext,jsobj, ida[i], &tokenObjectVal))
					{
						if (tokenObjectVal.isObject())
						{
							jsval nameValue;
							jsval typeValue;
							JS_GetProperty(esprimaContext, &tokenObjectVal.toObject(), "value", &nameValue);
							JS_GetProperty(esprimaContext, &tokenObjectVal.toObject(), "type", &typeValue);
							if (nameValue.isString() && typeValue.isString())
							{
								std::string typeString = JS_EncodeString(esprimaContext, typeValue.toString());
								std::string nameString = JS_EncodeString(esprimaContext, nameValue.toString());
								if (typeString == "Identifier")
								{
									int i = 0;
									BOOST_FOREACH(Rule currentRule, ruleSet)
									{										
										if(currentRule.getName() == nameString)
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

	typedef boost::adjacency_list<boost::vecS,boost::vecS,boost::directedS, Rule> CustomGraph;
	typedef boost::graph_traits<CustomGraph>::vertex_descriptor CustomVertex;
	typedef boost::graph_traits<CustomGraph>::edge_descriptor CustomEdge;
	CustomGraph customGraph;
	std::vector<CustomVertex> vertices;
	RulesDependencies(JSClass * classp)
	{
		JSRuntime *rt = JS_NewRuntime(8L * 1024 * 1024, JS_NO_HELPER_THREADS);
		esprimaContext = JS_NewContext(rt, 8192);
		esprimaGlobal = new JS::RootedObject(esprimaContext, JS_NewGlobalObject(esprimaContext, classp, nullptr));
		esprimaCompartement = new JSAutoCompartment(esprimaContext, *esprimaGlobal);
		JS_InitStandardClasses(esprimaContext, *esprimaGlobal);
	}

	void createRuleDependencies(std::vector<Rule> ruleSet, unsigned int ruleIndex)
	{
		Rule rule = ruleSet[ruleIndex];
		int lineno = 0;
		JS::Value rval;
		std::ifstream t("esprima.js");
		std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
		bool ok = JS_EvaluateScript(esprimaContext, *esprimaGlobal, str.c_str(), str.length(), "", lineno, &rval);
		{
			str = "esprima.tokenize(\""+rule.getCondition() +"\")";
			ok = JS_EvaluateScript(esprimaContext, *esprimaGlobal, str.c_str(), str.length(), "", lineno, &rval);
			if (!ok)
			{
				throw std::exception("Tokenize Rule not valid");
			}
			JS::RootedValue rootVal(esprimaContext,rval);
			parseTokenizeObject(ruleSet, ruleIndex, &rootVal);
		}
		{
			str = "esprima.tokenize(\""+rule.getAction() +"\")";
			ok = JS_EvaluateScript(esprimaContext, *esprimaGlobal, str.c_str(), str.length(), "", lineno, &rval);
			if (!ok)
			{
				throw std::exception("Tokenize Rule not valid");
			}
			JS::RootedValue rootVal(esprimaContext,rval);
			parseTokenizeObject(ruleSet, ruleIndex, &rootVal);
		}
	}

	void createGraphDependency(std::vector<Rule> ruleSet)
	{
		BOOST_FOREACH(Rule ruleToAddToGraph, ruleSet)
		{
			vertices.push_back(boost::add_vertex(ruleToAddToGraph, customGraph));
		}
		for(int index = 0; index<ruleSet.size();index++)
		{
			createRuleDependencies(ruleSet,index);
		}
	}

};