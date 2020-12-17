#ifndef STATISTICS_H
#define STATISTICS_H

#include<vector>
using std::vector;

// user-defined type Statistics
struct Statistics {
	// default constructor
	Statistics() noexcept
		: successes {}
		, failures {}{}

	// constructor taking number of successes and failures as arguments
	explicit Statistics(const vector<size_t>& successes, const vector<size_t>& failures) noexcept
		: successes { successes }
		, failures { failures }{}

	// statistics information
	vector<size_t> successes; // number of times questions were well answered
	vector<size_t> failures; // number of times questions were not well answered
};

// displays statistics menu
void display_updater_menu();

// statistics updater
[[maybe_unused]] void statistics_updater();

#endif
