#include "common.hpp"

po::variables_map parse_summary_command(po::parsed_options parsed);
void dump_samples(MAT::Tree &T, std::ostream &outfile);
void write_sample_table(MAT::Tree &T, std::string filename);
void write_clade_table(MAT::Tree &T, std::string filename);
void write_mutation_table(MAT::Tree &T, std::string filename);
void write_aberrant_table(MAT::Tree &T, std::string filename);
void summary_main(po::parsed_options parsed);
