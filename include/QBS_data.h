#pragma once
#include "QBS_window.h"
#include "QBS_consts.h"
#include "QBS_reporter.h"

#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <list>
#include <fstream>
#include <iostream>

#include "FL/Fl_Preferences.H"

class Fl_Input_Choice;

// Data structures

typedef std::map<std::string /* call */, int /*count*/> count_data;

struct recycle_data {
	int sum_recycled;            // Number of cards recycled from batch
	int sum_sent;                // Number of cards sent to ham
	int sum_received;            // Total number of cards in batch
	int count_recycled;          // Number of calls recycled
	int count_sent;              // Number of hamd to whom sent
	int count_received;          // Number of calls received
	float weight_kg;             // Weight (in kg) of cards recycled
	std::list<std::string> top_20;         // Top 20 callsigns recycled

	recycle_data() {
		sum_recycled = 0;
		sum_sent = 0;
		sum_received = 0;
		count_recycled = 0;
		count_sent = 0;
		count_received = 0;
		weight_kg = 0.0F;
		top_20.clear();
	}
};

struct box_data {
	std::string id;                   // eg "2020 Q2"
	std::string date_received;        // eg "2020-07-01(E)" - estimated
	std::string date_sent;            // or "2022-09-26" - actual date
	std::string date_recycled;        //
	recycle_data* recycle_info;  // Info for this box once disposed
	count_data* counts;          // Number of cards held or disposed
	count_data* sent;            // Number of cards sent
	count_data* received;        // Number of cards received

	box_data() {
		id = "Unknown";
		date_received = "";
		date_sent = "";
		date_recycled = "";
		recycle_info = new recycle_data;
		counts = new count_data;
		counts->clear();
		sent = new count_data;
		sent->clear();
		received = new count_data;
		received->clear();
	}
};

struct action_data {
	command_t command;           // Command implememted
	int box_num;                 // Box number (-1 == in-box)
	std::string date;                 // Date action occurs - may be estimated
	std::string call;                 // Call for batch
	int num_cards;               // Number of cards involved 
	int num_sases;               // Number of SASEs involved

};

struct note_data {
	std::string date;                 // Date of note
	std::string name;                 // Name of note
	std::string value;                // Value of note
};

typedef std::vector<note_data> notes;
typedef std::map<std::string, notes> call_info;

enum special_box_t {
	IN_BOX = -1,          // Box holding cards received outwith a batch      
	OUT_BOX = -2,         // "Box" representing cards placed in SASEs for posting
	KEEP_BOX = -3,        // "Box" representing cards removed from processed boxes 
	SASE_BOX = -4,        // Box containing SASEs
	RCVD_BOX = -5,        // "Box" representing ALL received cards
	SENT_BOX = -6,        // "Box" representing cards placed in post-box
	DISP_BOX = -7,        // "Box" representing cards placed in recycling bin  
	RCVD_ALL = -13,       // cards received for call in all boxes
	SENT_ALL = -14,       // cards sent for call in all boxes
	UNSENT_ALL = -15,     // cards not sent for call in all boxes

};

class QBS_window;

class QBS_data {

protected:
	// Data items

	// The main sorting boxes
	std::vector <box_data*> boxes_;
	// Other boxes
	count_data sases_; // SASEs
	// In-box - for items received between batches
	count_data in_box_;
	// Out box - items awaiting posting
	count_data out_box_;
	// Keep box - for cards kept between batches
	count_data keep_box_;
	// Disposal queue tail - most recent added to queue
	int tail_;
	// Disposal queue head - next to be recycled
	int head_;
	// CAllsign data
	call_info ham_info_;
	// "List" of calls (overall tally of cards for call)
	count_data received_box_;
	count_data sent_box_;
	count_data disposed_box_;
	count_data all_boxes_;
	// Raw data
	std::vector<action_data*> actions_;

	// Processing mode
	process_mode_t mode_;
	reading_mode_t reading_mode_;
	/*action_t*/ /*action_read_*/;

	// Filename
	std::string filename_;
	// CSV directory
	std::string csv_dirname_;
	// Window
	QBS_window* window_;
	// REprting window
	QBS_reporter* reporter_;
	// Dummy recycle data
	recycle_data recycle_dummy_;
	// Input/Output file
	std::fstream file_;

public:
	QBS_data();
	~QBS_data();

	inline process_mode_t mode() {
		return mode_;
	}
	inline void mode(process_mode_t value) {
		mode_ = value;
	}

	// Windows interface - get data
	// Get count for specific box/call - NB floating point as some "Boxes" represent averages
	int get_count(int box_num, std::string call);
	// Get current box_number
	int get_current();
	// Het dispose tal box_num
	int get_tail();
	// Get dispose head box_num
	int get_head();
	// Get batch name for box_number
	std::string get_batch(int box_num);
	// Get call in box - - return next one after this
	std::string get_next_call(int box_num, std::string call);
	std::string get_prev_call(int box_num, std::string call);
	std::string get_first_call(int box_num);
	std::string get_last_call(int box_num);
	// Get the count for a particular box
	count_data* get_count_data(int box_num);
	// Get recycle data
	recycle_data& get_recycle_data(int box_num);
	// Get box
	box_data* get_box(int box_num);
	// Set window
	void set_window(QBS_window* w);
	//// Get action
	//action_t get_action();
	// Get ham_info
	notes* get_notes(std::string callsign);

	// Load QBS file
	bool read_qbs(std::string& filename);
	// Load and parse .CSV files
	bool import_cvs(std::string& directory);
	// Open QBS file for writing
	bool wopen_qbs(std::string& filename);
	// Close QBS fule
	bool close_qbs();

	inline QBS_window* window() {
		return window_;
	}

	// Next batch ID
	std::string next_batch();


public:
	// Start cards to box
	int discard_inherits(
		std::string date,                // date received
		std::string call,                // callsign
		int value                   // num of cards
	);
	// Add cards to box
	int receive_cards(
		int box_num,                // box number
		std::string date,                // date received
		std::string call,                // callsign
		int value                   // num of cards
	);
	// ADd bew batch
	int new_batch(
		int box_num,                // box number
		std::string date,                // date created
		std::string batch                // batch "name" - e.g. "2022 Q4"
	);
	// Add/delete envelopes
	int receive_sases(
		std::string date,                // date actioned
		std::string call,                // callsign
		int value                   // num of envelopes
	);
	// send cards in envelopes
	int stuff_cards(
		int box_num,                // box number
		std::string date,                // date actioned
		std::string call,                // callsign
		int value                   // num of cards
	);
	// Use envelopes
	int use_sases(
		std::string date,                // date actioned
		std::string call,                // callsign
		int value                   // Number of envelopes
	);
	// keep cards
	int keep_cards(
		int box_num,                // box number
		std::string date,                // date actioned
		std::string call,                // callsign
		int value                   // num of cards
	);
	// Post cards - remove from out-tray to letter box
	int post_cards(
		std::string date                 // date actioned
	);
	// Dispose box - move current unsent to disposal queue
	int dispose_cards(
		std::string date                 // date actioned
	);
	// Recycle box - move head of disposal queue to blue bin
	int recycle_cards(
		std::string date,                // date actioned
		float weight                // weight of cards
	);
	// Totalise received cards
	int totalise_cards(
		int box_num,                // Box number
		std::string date                 // date actioned
	);
	// Add ham-data
	int ham_data(
		std::string date,                // Date
		std::string call,                // Callsign
		std::string name,                // Call info name
		std::string value                // Call info value
	);
	// Adjust box by value
	int adjust_cards(
		int box_num,                // Box number
		std::string date,                // date actioned
		std::string call,                // Callsign
		int delta                   // Value to adjust by
	);

	// Log the action
	void log_action(
		command_t command,
		int box_num,
		std::string date,
		std::string id,
		int i_value,
		float f_value
	);
	void log_action(
		command_t command,
		std::string date,
		std::string call,
		std::string name,
		std::string value
	);

	// Trace boxes
	void trace_boxes(std::ostream& os);

protected:
	// Prepare disposal report
	void evaluate_top20(
		int box_num             // box being disposed
	);

	// Print box summary
	void box_summary(int box, std::ostream& os);
	// Print SASE summary
	void sase_summary(std::ostream& os);

};
