#pragma once

#include <atomic>

// small event type
enum EvType
{
    EV_NOTE_ON  = 1,
    EV_NOTE_OFF = 2,
    EV_PARAM    = 3
};

struct Ev
{
    uint8_t type;
    uint8_t note; // Midi note value
    float   vel;  // Normalised velocity 0.. 1.0
    /* extra bytes for params */
};

class EvQueue
{
  public:
    EvQueue() {}
    ~EvQueue() = default;

    bool push(const Ev& e)
    {
        size_t h = head.load(std::memory_order_relaxed);
        size_t n = (h + 1) % N;
        if(n == tail.load(std::memory_order_acquire))
            return false;
        buf[h] = e;
        head.store(n, std::memory_order_release);
        return true;
    }

    bool pop(Ev& e)
    {
        size_t t = tail.load(std::memory_order_relaxed);
        if(t == head.load(std::memory_order_acquire))
            return false;
        e = buf[t];
        tail.store((t + 1) % N, std::memory_order_release);
        return true;
    }

  private:
    static constexpr size_t N = 256;
    Ev                      buf[N];
    std::atomic<size_t>     head{0}, tail{0};
};
