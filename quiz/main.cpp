#include "quiz.h"

#include<iostream>
using std::cin;
using std::cout;
using std::cerr;

#include<stdexcept>
using std::out_of_range;
using std::runtime_error;

int main()
try
{
	// creates required files
	create_file_if(questions_answers_file);
	create_file_if(resume_file);
	create_settings_file_if();

	// retrieves quiz information from file
	Quiz quiz = get_questions_and_answers();
	const vector<string>& questions = quiz.questions;
	const vector<string>& answers = quiz.answers;

	// retrieves resume information from file
	Resume resume = get_resume_information();
	const vector<size_t>& retry_indexes = resume.retry_indexes;
	
	// updates resume file
	update_resume_file(resume);

	// display main menu
	display_main_menu();

	// gets user's choice
	for(string choice; getline(cin, choice);){
		if (choice.length() != 1) choice = INVALID_CHOICE;
		else {
			// retrieves quiz information from file
			quiz = get_questions_and_answers();

			// retrieves resume information from file
			resume = get_resume_information();
		}

		switch (choice[0]) {
		case '1':
			if (questions.size()) {
				cout << '\n';
				list_elements(questions);
			}
			else
				cout << "\nThe list is empty.\n";
			break;

		case '2':
			cout << '\n';
			if (!questions.empty() && questions.size() == answers.size()) {
				if(resume.position == INVALID_POSITION) quiz_launcher(quiz, resume, Quiz::Mode::normal);
				else quiz_launcher(quiz, resume, Quiz::Mode::resume);
			}
			else {
				if(questions.empty()) cout << "There's not a single question to display.\n";
				else cout << "The number of questions doesn't match the number of answers.\n";
			}	
			break;

		case '3':
			cout << '\n';
			if (!retry_indexes.empty()) {
				quiz_launcher(quiz, resume, Quiz::Mode::practice);
			}
			else
				cout << "Please enter a valid choice.\n";
			break;

		case exit_character:
			break;

		default:
			cout << "\nPlease enter a valid choice.\n";
			break;
		}

		if (choice[0] == exit_character)
			break;
		else {
			cout << '\n';
			display_main_menu();
		}
	}

	cout << "\nGoodbye !\n";

	return 0;
}
catch (runtime_error& e) {
	cerr << "Error: " << e.what() << '\n';
	return 1;
}
catch (out_of_range& e) {
	cerr << "Error: " << e.what() << '\n';
	return 2;
}
catch (...) {
	cerr << "Error: unknown exception caught.\n";
	return 3;
}
