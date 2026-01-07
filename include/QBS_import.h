#pragma once
/***********************************************************************
*  QBS_import
* Provides a means of importing spreadsheet .CSV files into the database
*  QBS_column_table
* Widget displaying mapping between CSV column and DB field
*/
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <set>

#include "QBS_data.h"

class QBS_import 
{

public:

    QBS_import();
    virtual ~QBS_import();

    void clear_maps();

    bool load_data(QBS_data* data, const char* directory);

protected:

    // Read batch data
    bool read_batches();
    // Read card data
    bool read_card_data();
    // Read SASE data
    bool read_sase_data();
    // Read single batch
    bool read_batch(int box, std::string line);
    bool read_call(bool card, std::string line);

    // Copy internal data to QBS_data
    bool copy_data();

    // Directory name
    std::string directory_;
    // data
    QBS_data* data_;

    // Holding matrix - 3D: callsign x batch x direction
    // direction - fixed 4 entries
    // batches - defined after loading batches
    // callsign - dynamix increasing while loading cards or sases
    enum direction_t : char {
        RECEIVED = 0,
        SENT = 1,
        RECYCLED = 2,
        KEPT = 3,
        DIRN_INVALID
    };
    std::map<std::string, std::map<std::string, std::vector< int > > >  card_matrix_;
    std::map<std::string, std::map<std::string, std::vector< int > > >  sase_matrix_;
    int count(
        bool card,
        std::string call,
        std::string batch,
        direction_t dirn);

    // Holding batch data - 2D: data x batch + direction
    struct event_t {
        std::string batch;
        direction_t in_out{ RECEIVED };
    };
    std::map <std::string, event_t > dates_;
    // Map batch names to box number - initial batch is not a box (=-1)
    std::map <std::string, int> boxes_;
    std::map <int, std::string> batch_names_;
    // Weight of cards recycled from each batch (including initial)
    std::map <std::string, float> batch_weights_;
 
    // Holding call data
    call_info calls_;

    // File inputs
    std::ifstream in_;
    // Current file headers
    std::vector<std::string> columns_;
};

