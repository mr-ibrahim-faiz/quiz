#include "updater.h"
#include "quiz_launcher.h"

#include<iostream>
using std::cin;
using std::cout;
using std::cerr;

#include<stdexcept>
using std::exception;

// messages
const string message_empty_list { "The list is empty." };
const string message_no_questions { "There's not a single question to display." };
const string message_invalid_choice { "Please enter a valid choice." };
const string message_quiz_mismatch { "The number of questions doesn't match the number of answers." };
const string message_success_threshold_exceeded { "Well done. The Success threshold has been exceeded!" };
const string message_exit { "Goodbye !" };

int main()
try
{
	// creates required files
	initialize_quiz();

	// retrieves quiz information from file
	Quiz quiz = get_questions_and_answers();
	if (!quiz.removed.empty()) update_questions_answers_file();
	const vector<string>& questions = quiz.questions;
	const vector<string>& answers = quiz.answers;

	// retrieves statistics information from file
	Statistics statistics = get_statistics_information();
	statistics = update_statistics(statistics, quiz);
	update_statistics_file(statistics);

	vector<size_t> ignored_questions;

	// retrieves resume information from file
	Resume resume = get_resume_information();
	resume = update_resume(resume, quiz, statistics);
	update_resume_file(resume);
	const vector<size_t>& retry_indexes = resume.retry_indexes;

	// display main menu
	display_main_menu();

	// gets user's choice
	for(string choice; getline(cin, choice);){
		if (choice.length() != valid_choice_length) choice = INVALID_CHOICE;
		else {
			// retrieves quiz information from file
			quiz = get_questions_and_answers();
			if (!quiz.removed.empty()) update_questions_answers_file();

			// retrieves statistics information from file
			statistics = get_statistics_information();
			statistics = update_statistics(statistics, quiz);
			update_statistics_file(statistics);

			// retrieves resume information from file
			resume = get_resume_information();
			resume = update_resume(resume, quiz, statistics);
			update_resume_file(resume);

			// retrieves ignored questions
			ignored_questions = get_ignored_questions(statistics);
		}
		
		const char& user_choice = choice[0];	

		// clears screen
		if(user_choice != exit_choice) [[maybe_unused]] int result = system(clear_command.c_str());

		switch (user_choice) {
		case '1':
			if (questions.size()) list_elements(questions);
			else cout << message_empty_list << newline;
			cout << newline;
			break;

		case '2':
			if (!questions.empty() && questions.size() == answers.size()) {
				if(ignored_questions.size() == questions.size()) cout << message_success_threshold_exceeded << newline << newline;
				else if(resume.position == INVALID_POSITION) resume = quiz_launcher(quiz, resume, Quiz::Mode::normal);
				else resume = quiz_launcher(quiz, resume, Quiz::Mode::resume);
			}
			else {
				if(questions.empty()) cout << message_no_questions << newline << newline;
				else cout << message_quiz_mismatch << newline << newline;
			}
			break;

		case '3':
			if (!retry_indexes.empty() && questions.size() == answers.size()) {
				if (resume.retry_position == INVALID_POSITION) resume = quiz_launcher(quiz, resume, Quiz::Mode::practice_normal);
				else resume = quiz_launcher(quiz, resume, Quiz::Mode::practice_resume);
			}
			else cout << message_invalid_choice << newline << newline;
			break;

		case updater_choice:
		{
			// launches updater
			updater();

			// clears screen
			[[maybe_unused]] int result = system(clear_command.c_str());
		}
			break;

		case exit_choice:
			break;

		default:
			cout << message_invalid_choice << newline;
			cout << newline;
			break;
		}

		if (user_choice == exit_choice) break;
		else display_main_menu();
	}

	cout << newline;
	cout << message_exit << newline;

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
