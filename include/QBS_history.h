#pragma once

#include "zc_graph_.h"
#include <FL/Fl_Group.H>
#include <vector>

class zc_graph_bar_vertical;
class zc_zoom_scroll_bar;
class Fl_Box;
class Fl_Button;
class QBS_data;
class QBS_window;

class QBS_history :
	public Fl_Group
{
public:
	QBS_history(int X, int Y, int W, int H, const char* L = nullptr);
	void create_form();
	void enable_widgets();
	static void cb_done(Fl_Widget* w, void* v);
	static void cb_scroll(Fl_Widget* w, void* v);
	static void cb_execute(Fl_Widget* w, void* v);
	void data(QBS_data* d) { data_ = d; }

private:

	void copy_data_to_chart();

	void update_ranges(const zc_range<double>& range);	

	QBS_data* data_;
	QBS_window* win_;

	Fl_Button* bn_done_;
	Fl_Button* bn_execute_;
	zc_graph_bar_vertical* chart_;
	zc_zoom_scroll_bar* scroll_;
	Fl_Box* box_rcvd_;
	Fl_Box* box_sent_;
	Fl_Box* box_rcyc_;

	std::vector<zc_graph_::data_point_t> received_data_;
	std::vector<zc_graph_::data_point_t> sent_data_;
	std::vector<zc_graph_::data_point_t> recycled_data_;

};
