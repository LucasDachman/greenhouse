#include "SmoothingFilter.h"

SmoothingFilter::SmoothingFilter(uint16_t numSamples)
    : _numSamples(numSamples), _index(0), _filled(false)
{
  _samples = new int[_numSamples];
  _timestamps = new unsigned long[_numSamples];

  // Initialize both arrays to zero.
  for (uint16_t i = 0; i < _numSamples; i++)
  {
    _samples[i] = 0;
    _timestamps[i] = 0;
  }
}

SmoothingFilter::~SmoothingFilter()
{
  delete[] _samples;
  delete[] _timestamps;
}

void SmoothingFilter::addSample(int sample)
{
  // Store the current sample and record the current time.
  _samples[_index] = sample;
  _timestamps[_index] = millis();

  // Update the index as a circular buffer.
  _index = (_index + 1) % _numSamples;

  // Mark the buffer as filled if we've wrapped around.
  if (_index == 0)
  {
    _filled = true;
  }
}

int SmoothingFilter::getSmoothedValueWithin(unsigned long windowMs)
{
  unsigned long now = millis();
  uint32_t total = 0;
  uint16_t count = 0;

  // Determine the number of valid entries.
  uint16_t limit = _filled ? _numSamples : _index;

  // Sum samples that are not older than windowMs.
  for (uint16_t i = 0; i < limit; i++)
  {
    if (now - _timestamps[i] <= windowMs)
    {
      total += _samples[i];
      count++;
    }
  }

  // Return 0 if no samples qualify; otherwise, return their average.
  return (count > 0) ? (total / count) : -1;
}

int SmoothingFilter::getSmoothedValue()
{
  uint32_t total = 0;
  uint16_t count = _filled ? _numSamples : _index;

  // Sum all valid stored samples.
  for (uint16_t i = 0; i < count; i++)
  {
    total += _samples[i];
  }

  return (count > 0) ? (total / count) : -1;
}

void SmoothingFilter::reset()
{
  _index = 0;
  _filled = false;

  // Clear both the samples and timestamp arrays.
  for (uint16_t i = 0; i < _numSamples; i++)
  {
    _samples[i] = 0;
    _timestamps[i] = 0;
  }
}
