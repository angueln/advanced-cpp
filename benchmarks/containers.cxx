#include <celero/Celero.h>
#include <vector>
#include <list>
#include <deque>
#include <algorithm>
#include <set>
#include <unordered_set>

CELERO_MAIN

const int size = 1024;

BASELINE(Construction, PlainArrayInt, 64, 1024)
{
    celero::DoNotOptimizeAway(std::unique_ptr<int[]>(new int[size]));
}

BENCHMARK(Construction, InitArrayInt, 64, 1024)
{
    celero::DoNotOptimizeAway(std::unique_ptr<int[]>(new int[size] { 0 }));
}

BENCHMARK(Construction, VectorInt, 64, 1024)
{
    celero::DoNotOptimizeAway(std::vector<int>(size));
}

BENCHMARK(Construction, DequeInt, 64, 1024)
{
    celero::DoNotOptimizeAway(std::deque<int>(size));
}

BENCHMARK(Construction, ListInt, 64, 1024)
{
    celero::DoNotOptimizeAway(std::list<int>(size));
}

BASELINE(PushBack, PlainArrayInt, 64, 1024)
{
    std::unique_ptr<int[]> array(new int[size]);
    for (auto i = 0; i != size; ++i)
    {
        array[i] = i;
    }
    celero::DoNotOptimizeAway(array[0] + array[size-1]);
}

BENCHMARK(PushBack, VectorPushBack, 64, 1024)
{
    std::vector<int> v;
    for (auto i = 0; i != size; ++i)
    {
        v.push_back(i);
    }
    celero::DoNotOptimizeAway(v[0] + v[size-1]);
}

BENCHMARK(PushBack, VectorReservePushBack, 64, 1024)
{
    std::vector<int> v;
    v.reserve(size);
    for (auto i = 0; i != size; ++i)
    {
        v.push_back(i);
    }
    celero::DoNotOptimizeAway(v[0] + v[size-1]);
}

BENCHMARK(PushBack, DequePushBack, 64, 1024)
{
    std::deque<int> d;
    for (auto i = 0; i != size; ++i)
    {
        d.push_back(i);
    }
    celero::DoNotOptimizeAway(d[0] + d[size-1]);
}

BENCHMARK(PushBack, ListPushBack, 64, 1024)
{
    std::list<int> l;
    for (auto i = 0; i != size; ++i)
    {
        l.push_back(i);
    }
    celero::DoNotOptimizeAway(l.front() + l.back());
}

struct ContainerBenchmark : celero::TestFixture
{
    virtual std::vector<int64_t> getExperimentValues() const override
    {
        auto start = 4LL;
        std::vector<int64_t> values(13);
        std::generate(begin(values), end(values), [&start]() {
                return start *= 2;
        });
        return values;
    }
};

template <typename T>
struct IterateContainer : ContainerBenchmark
{
    virtual void setUp(int64_t size)
    {
        container_ = T(size);
        std::iota(begin(container_), end(container_), 0);
    }

    T container_;
};

template <typename T>
struct IterateContainer<std::unique_ptr<T[]>> : ContainerBenchmark
{
    virtual void setUp(int64_t size)
    {
        container_.reset(new T[size]);
        size_ = size;
        std::iota(container_.get(), container_.get() + size_, 0);
    }

    std::unique_ptr<T[]> container_;
    size_t size_;
};

/*
BASELINE_F(Iteration, PlainArray, IterateContainer<std::unique_ptr<int[]>>, 64, 1024)
{
    celero::DoNotOptimizeAway(std::accumulate(container_.get(), container_.get() + size_, 0));
}

BENCHMARK_F(Iteration, Vector, IterateContainer<std::vector<int>>, 64, 1024)
{
    celero::DoNotOptimizeAway(std::accumulate(begin(container_), end(container_), 0));
}

BENCHMARK_F(Iteration, Deque, IterateContainer<std::deque<int>>, 64, 1024)
{
    celero::DoNotOptimizeAway(std::accumulate(begin(container_), end(container_), 0));
}

BENCHMARK_F(Iteration, List, IterateContainer<std::list<int>>, 64, 1024)
{
    celero::DoNotOptimizeAway(std::accumulate(begin(container_), end(container_), 0));
}

*/

template <typename T>
struct SearchContainer : ContainerBenchmark
{
    virtual void setUp(int64_t size)
    {
        container_ = T(size);
        std::iota(begin(container_), end(container_), 0);
        needle_ = std::rand() % size;
    }

    T container_;
    typename T::value_type needle_;
};

template <typename T>
struct SearchContainer<std::unique_ptr<T[]>> : ContainerBenchmark
{
    virtual void setUp(int64_t size)
    {
        container_.reset(new T[size]);
        size_ = size;
        std::iota(container_.get(), container_.get() + size, 0);
        needle_ = std::rand() % size;
    }

    std::unique_ptr<T[]> container_;
    size_t size_;
    T needle_;
};

template <typename T>
struct SearchContainer<std::set<T>> : ContainerBenchmark
{
    virtual void setUp(int64_t size)
    {
        size_ = size;
        for (auto i = 0; i < size; ++i)
            container_.insert(i);
        needle_ = std::rand() % size;
    }

    std::set<T> container_;
    size_t size_;
    T needle_;
};


template <typename T>
struct SearchContainer<std::unordered_set<T>> : ContainerBenchmark
{
    virtual void setUp(int64_t size)
    {
        size_ = size;
        for (auto i = 0; i < size; ++i)
            container_.insert(i);
        needle_ = std::rand() % size;
    }

    std::unordered_set<T> container_;
    size_t size_;
    T needle_;
};

template <typename T>
struct SortContainer : ContainerBenchmark
{
    virtual void setUp(int64_t size)
    {
        container_ = T(size);
        std::generate(begin(container_), end(container_), std::rand);
        needle_ = std::rand();
    }

    T container_;
    typename T::value_type needle_;
};

template <typename T>
struct SortContainer<std::unique_ptr<T[]>> : ContainerBenchmark
{
    virtual void setUp(int64_t size)
    {
        container_.reset(new T[size]);
        size_ = size;
        std::generate(container_.get(), container_.get() + size_, std::rand);
        needle_ = std::rand();
    }

    std::unique_ptr<T[]> container_;
    size_t size_;
    T needle_;
};

/*

BASELINE_F(Sorting, PlainArray, SortContainer<std::unique_ptr<int[]>>, 128, 128)
{
    std::sort(container_.get(), container_.get() + size_);
}

BENCHMARK_F(Sorting, Vector, SortContainer<std::vector<int>>, 128, 128)
{
    sort(begin(container_), end(container_));
}

BENCHMARK_F(Sorting, Deque, SortContainer<std::deque<int>>, 128, 128)
{
    sort(begin(container_), end(container_));
}

BENCHMARK_F(Sorting, List, SortContainer<std::list<int>>, 128, 128)
{
    container_.sort();
}

BENCHMARK_F(Sorting, Deque, SortContainer<std::deque<int>>, 128, 128)
{
    sort(begin(container_), end(container_));
}

BENCHMARK_F(Sorting, List, SortContainer<std::list<int>>, 128, 128)
{
    container_.sort();
}
*/

BASELINE_F(Searching, PlainArray, SearchContainer<std::unique_ptr<int[]>>, 128, 2048)
{
    celero::DoNotOptimizeAway(std::find(container_.get(), container_.get() + size_, rand() % size_));
}

BENCHMARK_F(Searching, Vector, SearchContainer<std::vector<int>>, 128, 2048)
{
    celero::DoNotOptimizeAway(std::find(begin(container_), end(container_), rand() % container_.size()));
}

BENCHMARK_F(Searching, BinaryVector, SearchContainer<std::vector<int>>, 128, 2048)
{
    celero::DoNotOptimizeAway(std::binary_search(begin(container_), end(container_), rand() % container_.size()));
}

BENCHMARK_F(Searching, Set, SearchContainer<std::set<int>>, 128, 2048)
{
    celero::DoNotOptimizeAway(container_.find(rand() % size_));
}

BENCHMARK_F(Searching, UnorderedSet, SearchContainer<std::unordered_set<int>>, 128, 2048)
{
    celero::DoNotOptimizeAway(container_.find(rand() % size_));
}

template <typename T>
struct SortedContainer : SortContainer<T>
{
    virtual void setUp(int64_t size)
    {
        container_ = T(size);
        std::generate(begin(container_), end(container_), std::rand);
        needle_ = std::rand();
    }

    T container_;
    typename T::value_type needle_;
};


