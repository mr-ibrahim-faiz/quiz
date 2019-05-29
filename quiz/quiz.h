#ifndef QUIZ_H
#define QUIZ_H

#include<vector>
using std::vector;

#include<string>
using std::string;

// user-defined type Quiz
struct Quiz {
	// default constructor
	Quiz() noexcept
		: questions{}
		, answers{}{}

	// questions and answers
	vector<string> questions;
	vector<string> answers;

	// Modes
	enum class Mode {
		practice, normal, resume
	};
};

#endif
