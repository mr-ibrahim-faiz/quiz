#include "updater.h"
#include "quiz_launcher.h"

#include<iostream>
using std::cin;
using std::cout;

// constant expressions
constexpr size_t INITIAL_POSITION = 0;

// messages
const string message_list_cleared { "The ignored questions list has been cleared." };
const string message_invalid_choice { "Please enter a valid choice." };
const string message_position_set{ "The quiz position has been changed." };

// displays updater menu
void display_updater_menu(){
	cout << "[1] Include ignored questions\n";
	cout << "[2] Go to previous Quiz position.\n";
	cout << "[3] Go to previous Practice position.\n";
	cout << "[x] Exit\n";
}

// updater
void updater(){
	// retrieves quiz information from file
	Quiz quiz = get_questions_and_answers();
	if (!quiz.removed.empty()) update_questions_answers_file();

	// retrieves statistics information from file
	Statistics statistics = get_statistics_information();
	statistics = update_statistics(statistics, quiz);
	update_statistics_file(statistics);

	vector<size_t> ignored_questions;
	vector<size_t>& successes = statistics.successes;

	// retrieves resume information from file
	Resume resume = get_resume_information();
	resume = update_resume(resume, quiz, statistics);
	update_resume_file(resume);

	display_updater_menu();

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
			// clears the ignored questions list by reducing the success counts.
			while(ignored_questions.size()){
				for(const size_t& index: ignored_questions){
					successes[index] = successes[index] - 1;
				}

				// retrieves ignored questions
				ignored_questions = get_ignored_questions(statistics);
			}
			update_statistics_file(statistics);
			cout << message_list_cleared << newline << newline;
			break;

		case '2': case '3':
		{
			// sets the quiz position to the previous one
			// updates resume file
			Resume updated_resume = resume;
			Quiz::Mode mode = Quiz::Mode(user_choice - '2');

			if (is_practice(mode)) {
				if (resume.retry_position != INVALID_POSITION && resume.retry_position != INITIAL_POSITION) {
					updated_resume.retry_position = resume.retry_position - 1;
				}
			}
			else {
				if (resume.position != INVALID_POSITION && resume.position != INITIAL_POSITION) {
					updated_resume.position = resume.position - 1;
				}
			}

			updated_resume = update_resume(updated_resume, quiz, statistics);
			update_resume_file(updated_resume);
			cout << message_position_set << newline << newline;
		}
			break;

		case exit_choice:
			break;

		default:
			cout << message_invalid_choice << newline << newline;
			break;
		}

		if (user_choice == exit_choice) break;
		else display_updater_menu();
	}
}
