#include "QBS_data.h"
#include "QBS_window.h"
#include "QBS_consts.h"

#include <string>

#include <FL/Fl_Preferences.H>
#include <FL/Fl_PNG_Image.H>

using namespace std;

const char* DATE_FORMAT = "%Y-%m-%d";
extern int FL_NORMAL_SIZE;
const char* VERSION = "2.0.2";


QBS_window* window_;

void create_window(std::string filename) {
	char title[100];
	snprintf(title, 100, "QBS - GM4-8 QSL Bureau status - %s (%s)", VERSION, __DATE__);
	window_ = new QBS_window(400, 400, title, filename.c_str());
	printf("%s\n", title);
}

// The main app entry point
int main(int argc, char** argv)
{	
	// Change FL defaults
	FL_NORMAL_SIZE = 11;
	// First look for the icon in the smae directory as the executable
	Fl_PNG_Image* icon = new Fl_PNG_Image("qbs.png");
	if (icon->fail()) {
		// else look in
#ifdef _WIN32
		// C:\ProgramData\GM3ZZA\QBS
		std::string icon_file = std::string(getenv("ProgramData")) + "\\" + VENDOR + "\\" + PROGRAM_ID + "\\qbs.png";
#else
		// /etc/GM3ZZA/QBS
		std::string icon_file = "/etc/" + VENDOR + "/" + PROGRAM_ID + "/qbs.png";
#endif
		delete icon;
		icon = new Fl_PNG_Image(icon_file.c_str());
	}
	Fl_Window::default_icon(icon);

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


