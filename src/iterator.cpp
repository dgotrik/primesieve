///
/// @file  iterator.cpp
///
/// Copyright (C) 2018 Kim Walisch, <kim.walisch@gmail.com>
///
/// This file is distributed under the BSD License. See the COPYING
/// file in the top level directory.
///

#include <primesieve/iterator.hpp>
#include <primesieve/IteratorHelper.hpp>
#include <primesieve/PrimeGenerator.hpp>

#include <stdint.h>
#include <vector>
#include <memory>

namespace {

template <typename T>
void clear(std::unique_ptr<T>& ptr)
{
  ptr.reset(nullptr);
}

} // namespace

namespace primesieve {

iterator::~iterator()
{ }

iterator::iterator(uint64_t start,
                   uint64_t stop_hint)
{
  skipto(start, stop_hint);
}

void iterator::skipto(uint64_t start,
                      uint64_t stop_hint)
{
  start_ = start;
  stop_ = start;
  stop_hint_ = stop_hint;
  i_ = 0;
  last_idx_ = 0;
  dist_ = PrimeGenerator::maxCachedPrime();
  clear(primeGenerator_);
  primes_.clear();
}

void iterator::generate_next_primes()
{
  while (true)
  {
    if (!primeGenerator_)
    {
      IteratorHelper::next(&start_, &stop_, stop_hint_, &dist_);
      auto p = new PrimeGenerator(start_, stop_);
      primeGenerator_.reset(p);
      primes_.resize(64);
    }

    for (last_idx_ = 0; !last_idx_;)
      primeGenerator_->fill(primes_, &last_idx_);

    if (primeGenerator_->finished())
      clear(primeGenerator_);
    else
      break;
  }

  i_ = 0;
  last_idx_--;
}

void iterator::generate_prev_primes()
{
  if (primeGenerator_)
    start_ = primes_.front();

  primes_.clear();

  while (primes_.empty())
  {
    IteratorHelper::prev(&start_, &stop_, stop_hint_, &dist_);
    if (start_ <= 2)
      primes_.push_back(0);
    auto p = new PrimeGenerator(start_, stop_);
    primeGenerator_.reset(p);
    primeGenerator_->fill(primes_);
    clear(primeGenerator_);
  }

  last_idx_ = primes_.size() - 1;
  i_ = last_idx_;
}

} // namespace
