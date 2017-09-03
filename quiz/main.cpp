#include "quiz.h"

#include<iostream>
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

#include<stdexcept>
using std::out_of_range;
using std::runtime_error;

int main()
try
{
	// retrieves quiz information from files
	string questions_answers_file_address { "questions_answers.txt" };
	vector<string> questions;
	vector<string> answers;
	get_questions_and_answers(questions_answers_file_address, questions, answers);

	// initializes resume file address
	string resume_file_address { "resume_quiz.txt" };

	// display main menu
	display_main_menu(resume_file_address);

	// gets user's choice
	string choice { "" };
	while (true) {
		getline(cin, choice);

		if (choice.size() != 1)
			choice = "0";

		switch (choice[0]) {
		case '1':
			if (questions.size()) {
				cout << endl;
				list_elements(questions);
			}
			else
				cout << "\nThe list is empty." << endl;
			break;

		case '2':
			cout << endl;
			if (questions.size() > 0 && questions.size() == answers.size())
				quiz_launcher(questions, answers);
			else
				cout << "There's not a single question to display." << endl;
			break;

		case '3':
			cout << endl;
			if (are_questions_to_practice(resume_file_address)) {
				// gets retry indexes from resume file
				vector<size_t> indexes = get_retry_indexes(resume_file_address);

				// shuffle indexes
				shuffle_vector(indexes);

				simple_quiz_launcher(questions, answers, indexes);
			}
			else
				cout << "Please enter a valid choice." << endl;
			break;

		case 'x':
			break;

		default:
			cout << "\nPlease enter a valid choice." << endl;
			break;
		}

		if (choice[0] == 'x')
			break;
		else {
			cout << endl;
			display_main_menu(resume_file_address);
		}
	}

	cout << "\nGoodbye !" << endl;

	return 0;
}
catch (runtime_error& e) {
	cerr << "Error: " << e.what() << endl;
	return 1;
}
catch (out_of_range& e) {
	cerr << "Error: " << e.what() << endl;
	return 2;
}
catch (...) {
	cerr << "Error: unknown exception caught." << endl;
	return 3;
}