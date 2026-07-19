#include "QBS_history.h"

#include "QBS_charth.h"
#include "QBS_data.h"
#include "QBS_window.h"

#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>

#include "zc_fltk.h"
#include "zc_graph_.h"
#include "zc_range.h"

#include <string>
#include <vector>


const Fl_Color COLOUR_RECEIVED = FL_MAGENTA;
const Fl_Color COLOUR_RECYCLED = FL_RED;
const Fl_Color COLOUR_SENT = FL_GREEN;

QBS_history::QBS_history(int X, int Y, int W, int H, const char* L) :
	Fl_Group(X, Y, W, H, L),
	data_(nullptr),
	bn_done_(nullptr),
	chart_(nullptr),
	scroll_(nullptr),
	box_rcvd_(nullptr),
	box_sent_(nullptr),
	box_rcyc_(nullptr)
{
	win_ = zc::ancestor_view<QBS_window>(this);
	data_ = win_->data_;
	create_form();
	copy_data_to_chart();
}

void QBS_history::create_form() {
	align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT | FL_ALIGN_TOP);
	labelsize(FL_NORMAL_SIZE + 2);
	box(FL_BORDER_BOX);

	int cx = x() + GAP + WLABEL;
	int cy = y() + GAP + labelsize();
	int sy = cy;

	chart_ = new zc_graph_bar_vertical(cx, cy, 200, 200);
	chart_->box(FL_BORDER_BOX);
	chart_->textsize(FL_NORMAL_SIZE - 2);

	cy += chart_->h();


	// Add legend boxes
	box_rcvd_ = new Fl_Box(cx, cy, HBUTTON / 2, HBUTTON / 2, "RCVD");
	box_rcvd_->box(FL_FLAT_BOX);
	box_rcvd_->align(FL_ALIGN_RIGHT);
	box_rcvd_->color(COLOUR_RECEIVED);

	cx += WBUTTON;
	box_sent_ = new Fl_Box(cx, cy, HBUTTON / 2, HBUTTON / 2, "SENT");
	box_sent_->box(FL_FLAT_BOX);
	box_sent_->align(FL_ALIGN_RIGHT);
	box_sent_->color(COLOUR_SENT);

	cx += WBUTTON;
	box_rcyc_ = new Fl_Box(cx, cy, HBUTTON / 2, HBUTTON / 2, "RCYC");
	box_rcyc_->box(FL_FLAT_BOX);
	box_rcyc_->align(FL_ALIGN_RIGHT);
	box_rcyc_->color(COLOUR_RECYCLED);

	cy += HBUTTON / 2;
	cx = chart_->x();

	scroll_ = new zc_zoom_scroll_bar(cx, cy, 200, HBUTTON / 2);
	scroll_->type(FL_HORIZONTAL);
	scroll_->callback(cb_scroll, nullptr);

	cx = x() + w() - GAP - (2 * WBUTTON);
	cy = y() + h() - GAP - HBUTTON;

	bn_execute_ = new Fl_Button(cx, cy, WBUTTON, HBUTTON, "Execute");
	bn_execute_->callback(cb_execute, nullptr);
	bn_execute_->tooltip("Execute the current action");

	cx += WBUTTON;

	bn_done_ = new Fl_Button(cx, cy, WBUTTON, HBUTTON, "Done");
	bn_done_->callback(cb_done, nullptr);
	bn_done_->tooltip("Mark action done, and proceed to next action");

	end();

}

void QBS_history::enable_widgets() {
	if (data_) {
		bn_done_->activate();
	}
	else {
		bn_done_->deactivate();
	}
	if (data_) {
		copy_data_to_chart();
	}
	else {
		chart_->clear_data_sets();
	}
}

void QBS_history::copy_data_to_chart() {
	if (!data_) return;
	sent_data_.clear();
	recycled_data_.clear();

	chart_->clear_data_sets();
	int total_boxes = data_->get_current();
	if (total_boxes <= 0) return;

	chart_->start_config();
	chart_->clear_data_sets();
	chart_->set_axis_params(0);
	chart_->set_axis_params(1);
	chart_->set_axis_ranges(1, zc_range<double>(0, 10), zc_range<double>(0, 10000), zc_range<double>(0, 100));
	// For the first batch in every year, display the year in the label
	std::vector<std::string> labels;
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
	// Accumulate the data from each batch
	for (int b = 0; b <= total_boxes; b++) {
		box_data* box = data_->get_box(b);
		double sum_received = box->recycle_info->sum_received;
		double sum_sent = box->recycle_info->sum_sent;
		double sum_recycled = box->recycle_info->sum_recycled;
		received_data_.push_back({ b, sum_received });
		sent_data_.push_back({ b, sum_sent });
		recycled_data_.push_back({ b, sum_recycled });
	};
	chart_->add_data_set(1, &received_data_, { COLOUR_RECEIVED, 1, FL_SOLID });
	chart_->add_data_set(1, &sent_data_, { COLOUR_SENT, 1, FL_SOLID });
	chart_->add_data_set(1, &recycled_data_, { COLOUR_RECYCLED, 1, FL_SOLID });
	chart_->end_config();

	chart_->redraw();

	scroll_->bounds(zc_range<double>(0, total_boxes));
	scroll_->value(chart_->get_axis_range(0));

}

void QBS_history::update_ranges(const zc_range<double>& range) {
	chart_->set_axis_range(0, range);
	int box = 0;
	int max_value = 0;
	while (!range.contains(box) && box <= data_->get_current()) {
		box++;
	}
	while (range.contains(box)) {
		max_value = std::max(max_value, data_->get_box(static_cast<int>(box))->recycle_info->sum_received);
		box++;
	}
	chart_->set_axis_range(1, { 0, max_value * 1.1 });
	chart_->redraw();
}

void QBS_history::cb_scroll(Fl_Widget* w, void* v) {
	QBS_history* that = zc::ancestor_view<QBS_history>(w);
	zc_zoom_scroll_bar* bar = (zc_zoom_scroll_bar*)w;	
	zc_range<double> range = bar->value();
	that->update_ranges(range);
	that->chart_->redraw();
}

void QBS_history::cb_done(Fl_Widget* w, void* v) {
	QBS_history* that = zc::ancestor_view<QBS_history>(w);
	that->data_->mode(process_mode_t::DORMANT);
	that->win_->update_actions();
}

void QBS_history::cb_execute(Fl_Widget* w, void* v) {
	QBS_history* that = zc::ancestor_view<QBS_history>(w);
	switch (that->data_->mode()) {
	case process_mode_t::HISTORY:
		that->copy_data_to_chart();
		break;
	default:
		break;
	}
}
