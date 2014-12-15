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

#ifdef _MSC_VER
# define XP_WIN
#endif
#include <iostream>
#include "jsapi.h"
#include <string>
#include "headers/jsRetriever.h"
#include "headers/utils.h"
#include "headers/RulesDependencies.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp> 
#include <boost/graph/graphviz.hpp>

static JSClass global_class = {
    "global", 
    JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, 
    JS_DeletePropertyStub, 
    JS_PropertyStub, 
    JS_StrictPropertyStub,
    JS_EnumerateStub,  
    JS_ResolveStub, 
    JS_ConvertStub
};





int main(int argc, const char *argv[])
{

    JSRuntime *rt = JS_NewRuntime(8L * 1024 * 1024, JS_NO_HELPER_THREADS);
    if (!rt)
        return 1;

    	
	/*JS::RootedObject global(cx, JS_NewGlobalObject(cx, &global_class, nullptr));
    if (!global)
        return 1;

	{
      JS::Value rval;
	  JSAutoCompartment ac(cx, global); 
      JS_InitStandardClasses(cx, global);

      const char *script = "'hello'+'world, it is '+new Date()";
      const char *filename = "noname";
      int lineno = 0;
      JS_EvaluateScript(cx, global, script, strlen(script), filename, lineno, &rval);
      if (rval.isNull() | rval.isFalse() )
        return 1;

	}*/

  

	  /*std::wstring json = StringToWString(std::string("{\"id\" : 1}"));
	  JS::RootedValue val(cx);
	  JS_ParseJSON(cx, json.c_str(), json.length(), &val);*/

	Rule r00("r00","200 > 15 ", "r00 = 100");
	Rule r01("r01","r00 + 65 > 200 ", "r01 = 100");
	Rule r02("r02","132 > (r00 + r01)", "r02 = 100");
	



	std::vector<Rule*> ruleSet;
	ruleSet.push_back(&r00);
	ruleSet.push_back(&r01);
	ruleSet.push_back(&r02);
	RulesDependencies ruleDep(&global_class);
	ruleDep.createGraphDependency(ruleSet);
	


	// write graph to console
	std::cout << "\n-- graphviz output START --" << std::endl;
	boost::write_graphviz(std::cout, ruleDep.customGraph);
	std::cout << "\n-- graphviz output END --" << std::endl;
	JsRetriever * jsre = new JsRetriever(rt, &global_class);
	/*std::wstring json = get_file_contents("R0.rule");
	jsre->parseJSON(json);*/
	jsre->init();
	ruleDep.executeRuleSet(jsre);

   


	delete jsre;

	




    JS_DestroyRuntime(rt);
    JS_ShutDown();
    return 0;
}
