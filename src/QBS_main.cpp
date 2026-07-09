#include "QBS_data.h"
#include "QBS_window.h"
#include "QBS_consts.h"

#include "zc_file_holder.h"
#include "zc_fltk.h"

#include <string>

#include <FL/Fl_PNG_Image.H>

using namespace std;

const char* DATE_FORMAT = "%Y-%m-%d";
extern int FL_NORMAL_SIZE;
extern std::string APP_VERSION;
extern std::string APP_TIMESTAMP;
extern std::string APP_VENDOR;
extern std::string APP_NAME;

//! File holder customisation - control data
const std::map < uint8_t, file_control_t > FILE_CONTROL = {
	// ID, { filename, reference, read-only
	{ FILE_SETTINGS, { "QBS.json", false, false, 0 }},
	{ FILE_STATUS, { "status.txt", false, false, 0}},
	{ FILE_ICON_ZZA, { "rose.png", true, true, 0}}
};

QBS_window* window_;

void create_window(std::string filename) {
	char title[100];
	snprintf(title, 100, "%s: %s", APP_NAME.c_str(), APP_VERSION.c_str());
	window_ = new QBS_window(400, 400, title, filename.c_str());
	printf("%s\n", title);
}

// The main app entry point
int main(int argc, char** argv)
{	
	// Change FL defaults
	zc::customise_fltk();
	file_holder_ = new zc_file_holder(argv[0], FILE_CONTROL);

	// Get filename - use argument if set
	std::string filename = "";
	if (argc > 1) filename = std::string(argv[argc - 1]);
	// Create the window
	create_window(filename);
	window_->show(argc, argv);

	int code = 0;

	// Run the application until it is closed
	code = Fl::run();

	return code;

}


