#include "properties.h"

// mapping string with Property
map<const string, Property> mapping_string_property() {
	map<const string, Property> properties;

	properties["question"] = Property::question;
	properties["answer_index"] = Property::answer_index;
	properties["prompt"] = Property::prompt;

	return properties;
}
