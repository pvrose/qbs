#include "QBS_charth.h"
#include "QBS_data.h"

#include "zc_fltk.h"
#include "zc_graph_.h"
#include "zc_range.h"
#include "zc_zoom_scroll_bar.h"

#include <set>

#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/names.h>


using namespace std;

const Fl_Color COLOUR_RECEIVED = FL_MAGENTA;
const Fl_Color COLOUR_RECYCLED = FL_RED;
const Fl_Color COLOUR_SENT = FL_GREEN;

QBS_charth::QBS_charth(int X, int Y, int W, int H, const char* L) :
	Fl_Group(X, Y, W, H, L)
{
	data_ = nullptr;
	max_ = 15;
	win_tip_ = nullptr;
	create_form();
	update("");
}

QBS_charth::~QBS_charth() {

}

void QBS_charth::create_form() {
	box(FL_BORDER_BOX);

	int cx = x();
	int cy = y();
	int ch = h() - HBUTTON;
	chart_ = new zc_graph_bar_vertical(cx, cy, w(), ch);
	chart_->box(FL_BORDER_BOX);
	chart_->textsize(FL_NORMAL_SIZE - 2);

	cy += ch;

	// Add legend boxes
	box_rcvd_ = new Fl_Box(cx, cy, HBUTTON / 2, HBUTTON / 2, "RCVD");
	box_rcvd_->box(FL_FLAT_BOX);
	box_rcvd_->align(FL_ALIGN_RIGHT);
	box_rcvd_->color(COLOUR_RECEIVED);

	cx += w() / 3;
	box_sent_ = new Fl_Box(cx, cy, HBUTTON / 2, HBUTTON / 2, "SENT");
	box_sent_->box(FL_FLAT_BOX);
	box_sent_->align(FL_ALIGN_RIGHT);
	box_sent_->color(COLOUR_SENT);

	cx += w() / 3;
	box_rcyc_ = new Fl_Box(cx, cy, HBUTTON / 2, HBUTTON / 2, "RCYC");
	box_rcyc_->box(FL_FLAT_BOX);
	box_rcyc_->align(FL_ALIGN_RIGHT);
	box_rcyc_->color(COLOUR_RECYCLED);

	cy += HBUTTON / 2;
	cx = x();

	scroll_ = new zc_zoom_scroll_bar(cx, cy, w(), HBUTTON / 2);
	scroll_->type(FL_HORIZONTAL);
	scroll_->callback(cb_scroll, nullptr);

	end();
}

void QBS_charth::data(QBS_data* d) {
	data_ = d;
}

void QBS_charth::update(std::string call) {
	call_ = call;
	set_chart();
}

// Set the chart data from the QBS_data object
void QBS_charth::set_chart() {
	int total_boxes = 0;
	chart_->start_config();
	chart_->clear_data_sets();
	chart_->set_axis_params(0);
	chart_->set_axis_params(1);
	chart_->set_axis_ranges(1, zc_range<double>(0, 5), zc_range<double>(0, 100), zc_range<double>(0, 15));
	
	if (data_) {
		// Get the number of boxes to display
		total_boxes = data_->get_current();
		if (total_boxes > 0) {
			std::vector<std::string> labels;
			// For the first batch in every year, display the year in the label
			for (int b = 0; b <= total_boxes; b++) {
				std::string box_name = data_->get_batch(b);
				if (box_name[6] == '1') {
					labels.push_back(box_name.substr(2, 2));
				}
				else {
					labels.push_back("");
				}
			}
			chart_->set_bar_labels(0, labels);
			received_data_.clear();
			sent_data_.clear();
			recycled_data_.clear();
			double average_rcvd = 0.0;
			// Add the data to the chart
			for (int b = 0; b <= total_boxes; b++) {
				box_data* box = data_->get_box(b);
				int rcvd = 0;
				if (box->received->find(call_) != box->received->end()) {
					rcvd = box->received->at(call_);
				}
				average_rcvd += rcvd;
				int sent = 0;
				if (box->sent->find(call_) != box->sent->end()) {
					sent = box->sent->at(call_);
				}
				int rcyc = 0;
				if (box->counts->find(call_) != box->counts->end()) {
					rcyc = box->counts->at(call_);
				}
				received_data_.push_back(std::make_pair((double)b, (double)rcvd));
				sent_data_.push_back(std::make_pair((double)b, (double)sent));
				recycled_data_.push_back(std::make_pair((double)b, (double)rcyc));
			}
			chart_->add_data_set(1, &received_data_, { COLOUR_RECEIVED, 1, FL_SOLID });
			chart_->add_data_set(1, &sent_data_, { COLOUR_SENT, 1, FL_SOLID });
			chart_->add_data_set(1, &recycled_data_, { COLOUR_RECYCLED, 1, FL_SOLID });
			average_rcvd /= (double)(total_boxes + 1);
			chart_->add_marker(1, zc_graph_::FOREGROUND, { FL_BLUE, 1, FL_DASHDOT }, average_rcvd);
		}
		scroll_->bounds(zc_range<double>(0, total_boxes));
	}
	chart_->end_config();
	// Reduce the range to the last 12 boxes if there are more than 12 boxes
	// \note this can only be done after end_config().
	if (total_boxes > 12) {
		chart_->set_axis_range(0, zc_range<double>(total_boxes - 12, total_boxes));
	}
	chart_->redraw();

	scroll_->value(chart_->get_axis_range(0));
}

/*
//Place holder for now
int QBS_charth::handle(int event) {
	switch(event) {
		case FL_PUSH: {
			int x = Fl::event_x() - chart_->x();
			int y = Fl::event_y() - chart_->y();
			if (x > 0 && x < chart_->w() &&
				y > 0 && y < chart_->h()) {
				chart_tip();
				return true;
			}
		}
		case FL_RELEASE: {
			if (win_tip_)  {
				Fl::delete_widget(win_tip_);
				win_tip_ = nullptr;
				return true;
			}
		}
	}
	return Fl_Group::handle(event);
}
*/
/*
void QBS_charth::chart_tip() {
	int x = Fl::event_x() - chart_->x();
	float bar_width = (float)chart_->w() / (float)number_boxes_;
	int bar = (int)trunc((float)x / bar_width);
	int rcvd = chart_counts_[bar * 4];
	int rcyc = chart_counts_[bar * 4 + 1];
	int sent = chart_counts_[bar * 4 + 2];
	int i_box = start_box_ + bar;
	std::string id = data_->get_box(i_box)->id;

	// Create text for tooltip
	char text[256];
	snprintf(text, sizeof(text), "Box %d %s\nReceived %d\nSent %d\nRecycled %d",
		i_box, id.c_str(), rcvd, sent, rcyc); 
	// get the size of the text, set the font, default width
	fl_font(Fl_Tooltip::font(), Fl_Tooltip::size());
	int width = Fl_Tooltip::wrap_width();
	int height = 0;
	// Now get the actual width and height
	fl_measure(text, width, height, 0);
	// adjust sizes to allow for margins - use Fl_Tooltip margins.
	width += (2 * Fl_Tooltip::margin_width());
	height += (2 * Fl_Tooltip::margin_height());
	// Create the window
	win_tip_ = new Fl_Window(Fl::event_x_root(), Fl::event_y_root(), width, height, 0);
	win_tip_->clear_border();
	
	// Create the output widget.
	Fl_Multiline_Output* op = new Fl_Multiline_Output(0, 0, width, height, 0);
	// Copy the attributes of tool-tips
	op->color(Fl_Tooltip::color());
	op->textcolor(Fl_Tooltip::textcolor());
	op->textfont(Fl_Tooltip::font());
	op->textsize(Fl_Tooltip::size());
	op->wrap(true);
	op->box(FL_BORDER_BOX);
	op->value(text);
	win_tip_->add(op);
	win_tip_->end();
	// set the window parameters: always on top, tooltip
	win_tip_->set_non_modal();
	win_tip_->set_tooltip_window();
	// Must be after set_tooltip_window.
	win_tip_->show();
}
*/

void QBS_charth::cb_scroll(Fl_Widget* w, void* v) {
	QBS_charth* that = zc::ancestor_view<QBS_charth>(w);
	zc_zoom_scroll_bar* bar = (zc_zoom_scroll_bar*)w;	zc_range<double> range = bar->value();
	that->chart_->set_axis_range(0, range);
	that->chart_->redraw();
}