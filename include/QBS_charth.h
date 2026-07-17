#pragma once

#include <string>
#include <vector>

#include "zc_graph_.h"
#include "zc_zoom_scroll_bar.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>

class QBS_data;

class QBS_charth :
    public Fl_Group
{

public:
    QBS_charth(int X, int Y, int W, int H, const char* L = nullptr);
    ~QBS_charth();

    void create_form();

    void data(QBS_data* d);

    void update(std::string call);

//    virtual int handle(int event);

    static void cb_scroll(Fl_Widget* w, void* v);

protected:

    QBS_data* data_;

    // Widgets
    zc_graph_bar_vertical* chart_;
    zc_zoom_scroll_bar* scroll_;
    Fl_Box* box_rcvd_;
    Fl_Box* box_sent_;
	Fl_Box* box_rcyc_;

    Fl_Window* win_tip_;

    // Create a tooltip for chart bar
//    void chart_tip();

	// Copy data to chart for drawing
	void set_chart();

    int max_;
    // average through range
    double average_;

    std::string call_;
    
    std::vector<int> chart_counts_;

    std::vector<zc_graph_::data_point_t> received_data_;
    std::vector<zc_graph_::data_point_t> sent_data_;
    std::vector<zc_graph_::data_point_t> recycled_data_;

};

