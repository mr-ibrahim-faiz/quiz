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

	// removed questions and answers
	vector<size_t> removed;

	// Modes
	enum class Mode {
		normal, practice_normal, resume, practice_resume
	};
};

#endif
