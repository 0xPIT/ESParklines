//
// ESParklines – I <3 Sparklines!
// Sparklines for ESP8266/ESP32/Arduino Displays
//
// Copyright (c) 2020 karl@pitrich.com
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include <string.h>
#include <functional>
#include <algorithm>
#include <FixedPoints.h>
#include <FixedPointsCommon.h>

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
class SparkLine
{
  using drawLineFunction = std::function<void(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)>;
  using Point = struct { uint16_t x; uint16_t y; };  
  using num_t = SFixed<15, 16>;

  T* container;
  const size_t capacity;
  size_t elements;
  drawLineFunction drawLine;

  T findMin() const {
    T lo = container[0];
    for (size_t i = 0; i < elements; i++) {
      if (container[i] < lo) lo = container[i];
    }
    return lo;
  }

  T findMax() const {
    T hi = 0;
    for (size_t i = 0; i < elements; i++) {
      if (container[i] > hi) hi = container[i];
    }
    return hi;
  }

public:
  SparkLine(size_t _size, drawLineFunction _dlf)
    : container(new T[_size]),
      capacity(_size),
      drawLine(_dlf)
  {
    reset();
  }

  virtual ~SparkLine()
  {
    delete[] container;
  }

  void reset() 
  {
    memset(container, 0, capacity * sizeof(T));
    elements = 0;
  }

  void add(T value) 
  {
    if (elements < capacity) {
      container[elements] = value;
      elements++;
      return;
    }
    
    memmove(container, &container[1], (capacity - 1) * sizeof(T));
    container[capacity - 1] = value;
  }

  
  /**
   * Draw Sparkline using passed drawLine function
   * 
   * @param x           left-most poont where to start the sparkline
   * @param y           bottom point of the sparkline – to match font redering
   * @param maxWidth    max width of the sparkline 
   * @param maxHeight   max height of the sparkline 
   * @param lineWidth   width of the stroke for the line to be drawn
   */
  void draw(uint16_t x, uint16_t y, 
            uint16_t maxWidth, uint16_t maxHeight, 
            uint16_t lineWidth = 1) const
  {
    if (elements < 2) {
      return;
    }

    num_t slope = 1.0f * (maxHeight - lineWidth);

    T lo = this->findMin();
    T hi = this->findMax();
    if (lo != hi) {
      slope /= hi - lo;
    }
    
    Point lastPoint = { 0, 0 };
    num_t segment = 0.0f;
    
    size_t maxSegments = elements;
    if (maxSegments > maxWidth) {
      maxSegments = maxWidth;
    }

    num_t pixelPerSegment = 1.0f;
    if (elements <= maxWidth) {
      pixelPerSegment = 1.0f * maxWidth / (elements - 1);
    }

    for (size_t i = 0; i < maxSegments; i++) {
      T value = container[i];
      num_t scaledValue = maxHeight - ((value - lo) * slope);
      scaledValue -= lineWidth / 2.0f;
      scaledValue += y - maxHeight;

      Point pt {
        .x = static_cast<uint16_t>(x + segment.getInteger()),
        .y = static_cast<uint16_t>(scaledValue.getInteger())
      };

      if (segment > 0) {
        drawLine(lastPoint.x, lastPoint.y, pt.x, pt.y);
      }

      lastPoint = pt;
      segment += pixelPerSegment;
    }
  }
};
