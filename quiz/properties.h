#ifndef PROPERTIES_H
#define PROPERTIES_H

#include<map>
using std::map;

#include<string>
using std::string;

// user-defined type Property
enum class Property {
	invalid=-1, question, answer_index, prompt
};

// mapping string with Property
map<const string, Property> mapping_string_property();

#endif
