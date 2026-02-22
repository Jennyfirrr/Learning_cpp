// So there was one leetcode problem that I did, where I said that I wasnt sure
// why the optimal solution was faster, SO i decided to look into it.
//
// To start off, when you know the entire vector size, like 1 to n length, its
// faster to precalculate and reserve that memory becaues its like o(1) or o(n)
// or something, and it will never change, because you already know how much you
// need, whereas when you use the vector.push_back(n) operator, it has to
// contintually call a copy, or move, and delete operator, beceause if i
// remember right from what i read, it cant jsut dynamically update the
// locations in memory that it is using, which leads to far more compute time
// than would take by just declaring the entire length you know you will need.
//
// In a similar
// problem
// (https://github.com/Jennyfirrr/Learning_cpp/blob/main/projects/leetcode/missing_nums_1toN.cpp),
// I was using something like vector[nums[i]] to use the value of nums at index
// i, as the index for a counter in the vector array, so like, if nums[1] was 2,
// then the vector[2] array would get a +1 to the value, which while it works
// for finding duplicates, or missing numbers, it also takes far more compute in
// some situations, because you have to perform an entire math operation cycle
// to update the number, when for even faster operations, you can just use
// vector[nums[i]] = 1, to signify that the number of that value exists, this
// doesnt really work for counting the amount of times you see it, but for other
// problems where you just need to verify that that number has been seen, this
// is way faster because you just directly overwrite the existing value with 1,
// instead of performing a math operation cycle.
