class RangeChecker {
public:
    bool check_in_window(int target, int base);
    bool check_in_range(int target, int lb, int ub);

private:
    int _window_size;
    int _max_seqnum;
};