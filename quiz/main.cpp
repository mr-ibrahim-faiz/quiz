#include "quiz_launcher.h"

#include<iostream>
using std::cin;
using std::cout;
using std::cerr;

#include<stdexcept>
using std::exception;

int main()
try
{
	// retrieves quiz information from file
	Quiz quiz = get_questions_and_answers();
	if (!quiz.removed.empty()) update_questions_answers_file();
	const vector<string>& questions = quiz.questions;
	const vector<string>& answers = quiz.answers;

	// retrieves resume information from file
	Resume resume = get_resume_information();
	resume = update_resume(resume, quiz);
	const vector<size_t>& retry_indexes = resume.retry_indexes;
	
	// sets up resume file
	update_resume_file(resume);

	// display main menu
	display_main_menu();

	// gets user's choice
	for(string choice; getline(cin, choice);){
		if (choice.length() != valid_choice_length) choice = INVALID_CHOICE;
		else {
			// retrieves quiz information from file
			quiz = get_questions_and_answers();
			if (!quiz.removed.empty()) update_questions_answers_file();

			// retrieves resume information from file
			resume = get_resume_information();
			resume = update_resume(resume, quiz);
		}
		
		const char& user_choice = choice[0];	
		cout << newline;

		switch (user_choice) {
		case '1':
			if (questions.size()) list_elements(questions);
			else cout << "The list is empty.\n";
			break;

		case '2':
			if (!questions.empty() && questions.size() == answers.size()) {
				if(resume.position == INVALID_POSITION) resume = quiz_launcher(quiz, resume, Quiz::Mode::normal);
				else resume = quiz_launcher(quiz, resume, Quiz::Mode::resume);
			}
			else {
				if(questions.empty()) cout << "There's not a single question to display.\n";
				else cout << "The number of questions doesn't match the number of answers.\n";
			}	
			break;

		case '3':
			if (!retry_indexes.empty() && questions.size() == answers.size()) {
				if (resume.retry_position == INVALID_POSITION) resume = quiz_launcher(quiz, resume, Quiz::Mode::practice_normal);
				else resume = quiz_launcher(quiz, resume, Quiz::Mode::practice_resume);
			}
			else cout << "Please enter a valid choice.\n";
			break;

		case exit_character:
			break;

		default:
			cout << "Please enter a valid choice.\n";
			break;
		}

		if (user_choice == exit_character) break;
		else {
			cout << newline;
			display_main_menu();
		}
	}

	cout << "Goodbye !\n";

	return 0;
}
catch (const exception& e) {
	cerr << "Error: " << e.what() << newline;
	return 1;
}
catch (...) {
	cerr << "Error: unknown exception caught.\n";
	return 2;
}
