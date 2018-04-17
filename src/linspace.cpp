#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <getopt.h>
#include "distmat/distmat.h"
#include "bonsai/kspp/ks.h"

dm::DistanceMatrix<float, 1> parse_flashdans_matrix(const std::string &path) {
    std::ifstream ifs(path);
    std::string line;
    if(!std::getline(ifs, line)) throw "a party!";
    std::vector<size_t> offsets;
    ks::split(line.data(), '\t', offsets);
    dm::DistanceMatrix<float, 1> ret(offsets.size() - 1);
    std::fprintf(stderr, "Size: %zu\n", offsets.size());
    size_t linenum = 0;
    while(std::getline(ifs, line)) {
        offsets.clear();
        ks::split(line.data(), '\t', offsets);
        for(size_t i(1); i < offsets.size(); ++i) {
            if(std::strcmp(line.data() + offsets[i], "-")) {
                ret(linenum, i - 1) = std::atof(line.data() + offsets[i]);
            }
        }
    }
    return ret;
}

std::vector<float> linspace(size_t ndiv) {
    std::vector<float> ret; ret.reserve(ndiv + 1);
    for(size_t i(0); i < ndiv; ret.emplace_back(static_cast<float>(i) / ndiv));
    return ret;
}

void usage(const char *ex) {
    std::fprintf(stderr, "%s -n <ndiv [default: 40]> -S <subset size [default: 40]> distmat.txt\n", ex);
    std::exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int c;
    unsigned ndiv = 100, subset_size = 40;
    if(argc == 1) goto usage;
    while((c = getopt(argc, argv, "n:s:h?")) >= 0) {
        switch(c) {
            case 'n': ndiv = std::atoi(optarg); break;
            case 's': subset_size = std::atoi(optarg); break;
            case 'h': case '?': usage: usage(*argv);
        }
    }
    auto mat = parse_flashdans_matrix(argv[1]);
    auto lins = linspace(ndiv);
    std::vector<size_t> pair_counts(ndiv, static_cast<size_t>(0));
    size_t i(0);
    while(std::accumulate(std::cbegin(pair_counts), std::cend(pair_counts), static_cast<size_t>(0), [](auto a, auto b) {return a + (b != 0);}) < ndiv) {
        for(size_t j(0); j < i; ++pair_counts[static_cast<size_t>(ndiv * mat(i, j++))]);
        ++i;
    }
    std::fprintf(stderr, "[W:%s:%s:%d] Currently not using subset_size (value: %u)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, subset_size);
    //std::unordered_set<size_t> indices;
    //while(indices.size() < i) indices.push_back(indices.size());
    auto tinymat = dm::DistanceMatrix<float, 1>(i);
    for(size_t j(0); j < i; ++j) {
        for(size_t k(j + 1); k < i; ++j) {
            tinymat(i, j) = mat(i, j);
        }
    }
    tinymat.printf(stdout);
}
