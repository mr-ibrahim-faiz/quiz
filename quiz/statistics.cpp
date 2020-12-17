#include "quiz_launcher.h"

#include<iostream>
using std::cin;
using std::cout;

// displays statistics menu
void display_updater_menu(){
	cout << "[1] Include ignored questions\n";
	cout << "[x] Exit\n";
}

// statistics updater
void statistics_updater(){
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
			// clears the ignored questions list by reducing every success count by 1.
			while(ignored_questions.size()){
				for(const size_t& index: ignored_questions){
					successes[index] = successes[index] - 1;
				}

				// retrieves ignored questions
				ignored_questions = get_ignored_questions(statistics);
			}
			update_statistics_file(statistics);
			cout << "The ignored questions list has been cleared.\n\n";
			break;

		case exit_choice:
			break;

		default:
			cout << "Please enter a valid choice.\n\n";
			break;
		}

		if (user_choice == exit_choice) break;
		else display_updater_menu();
	}
}
