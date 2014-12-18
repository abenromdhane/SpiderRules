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
  void discover_vertex(CustomVertex v, const CustomGraph& g) const;
  JsRetriever* jsRetriever;
};

class RulesSet
{
private :
	JsRetriever* esprimaRetriever;
	JSRuntime *rt;
	JsRetriever* rulesExecutioner;
	std::vector<Rule*> rulesVector;

	void parseTokenizeObject(unsigned int ruleIndex, JS::MutableHandleValue rootVal);
public :


	CustomGraph customGraph;
	std::vector<CustomVertex> vertices;
	RulesSet(JSClass * classp);

	void setInputObject(std::wstring json);
	

	void createRuleDependencies(unsigned int ruleIndex);

	void addRule(Rule* rule);

	void createGraphDependency();

	void executeRulesSet();

	~RulesSet();

	void printRulesResult();

	void bindInputData(std::wstring json);
};