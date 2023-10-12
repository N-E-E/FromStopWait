#include "Helpers.h"

bool RangeChecker::check_in_range(int target, int lb, int ub) {
    if (lb <= ub) {
        return target >= lb && target < ub;
    } else {
        if (target >= lb) return target < ub + _max_seqnum;
        else return target < ub;
    }
    return false;
}

bool RangeChecker::check_in_window(int target, int base) {
    if (target < base) {
        return target < (base + _window_size) % _max_seqnum;
    } else {
        return target < base + _window_size;
    }
}