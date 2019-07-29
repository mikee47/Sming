/*
 * test_HashMap.cpp
 *
 * @author: 21 Feb 2019 - mikee47 <mike@sillyhouse.net>
 */

#include <Data/ObjectMap.h>
#include <WString.h>

class TestClass
{
public:
	TestClass()
	{
		debug_i("TestClass() %p", this);
	}

	~TestClass()
	{
		debug_i("~TestClass() %p", this);
	}
};

typedef ObjectMap<String, TestClass> TestMap;

static void constTest(const TestMap* map)
{
	//  auto& value = (*map)["non-existent-key"];
}

void testObjectMap()
{
	TestMap map;

	for(unsigned i = 0; i < 5; ++i) {
		String key = "Object " + String(i);
		debug_i("map[\"%s\"] = new", key.c_str());
		map[key] = new TestClass;
	}

	for(unsigned i = 0; i < 5; ++i) {
		String key = "Object " + String(i);
		debug_i("map[\"%s\"] = new", key.c_str());
		map[key] = new TestClass;
	}

	// Test non-existent values
	auto value = map["non existent key"];
	debug_i("map[\"%s\"] = %p", value.getKey().c_str(), (void*)value);
	debug_i("map.count() = %u", map.count());
	value = new TestClass;
	debug_i("map.count() = %u", map.count());

	debug_i("extract");
	auto obj = map.extract("Object 0");
	debug_i("\"Object 0\" -> %p", obj);
	delete obj;

	debug_i("done");

	//  for(unsigned i = 0; i < 5; ++i) {
	//    String key = "Object " + String(i);
	//    debug_i("map[\"%s\"] = nullptr", key.c_str());
	//    map[key] = nullptr;
	//  }

	//	while(map.count() != 0) {
	//    debug_i("ObjectMap.removeAt(0)");
	//	  map.removeAt(0);
	//	}

	constTest(&map);
}
