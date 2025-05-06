#ifndef SMOOTHINGFILTER_H
#define SMOOTHINGFILTER_H

#include <Arduino.h>

class SmoothingFilter
{
public:
  // Constructor: numSamples determines how many readings to keep (the buffer size).
  SmoothingFilter(uint16_t numSamples);

  // Destructor frees dynamic memory.
  ~SmoothingFilter();

  // Adds a new sample (e.g., brightness) along with its timestamp (using millis()).
  void addSample(int sample);

  // Returns the average of samples within the last windowMs milliseconds.
  int getSmoothedValueWithin(unsigned long windowMs);

  // Returns the average of all stored samples (ignoring timestamps).
  int getSmoothedValue();

  // Resets the filter, clearing all stored samples.
  void reset();

private:
  uint16_t _numSamples;       // Buffer size: maximum number of stored samples.
  int *_samples;              // Dynamic array storing the sample values.
  unsigned long *_timestamps; // Dynamic array storing the timestamp for each sample.
  uint16_t _index;            // Current position in the circular buffer.
  bool _filled;               // Indicates if the buffer has been filled at least once.
};

#endif
