#ifndef SMOOTHED_PARAM_H
#define SMOOTHED_PARAM_H

class SmoothedParam
{
  public:
    SmoothedParam(float smoothRate = 0.001f) : smoothRate_{smoothRate} {}

    void Set(float x) { target_ = x; }

    float Process()
    {
        current_ += smoothRate_ * (target_ - current_);
        return current_;
    }

  private:
    float target_;
    float current_;
    float smoothRate_;
};

#endif /* SMOOTHED_PARAM_H */
